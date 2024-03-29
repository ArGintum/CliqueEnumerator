/*
Clique Enumerator : a parallel algorithm for fast enumeration of cliques in a graph

MIT License

Copyright (c) 2021 Eduard Tulchinskii

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <boost/python.hpp>


#include <algorithm>
#include <atomic>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>

#include <boost/graph/adjacency_list.hpp>

#include <boost/python/def.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/module.hpp>



typedef int V;
typedef std::pair<V, V> E;
typedef std::vector<V> Clique;

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> Graph;

void build_k_clique(const Graph& graph, unsigned int k, Clique current_clique, std::vector<V>& candidates, std::vector<Clique>& cliques) {
    if (2 < current_clique.size() && current_clique.size() <= k) {
        cliques.push_back(current_clique);
    }
    if (current_clique.size() >= k) {
        return;
    }

    Clique new_clique = current_clique;
    new_clique.push_back(0);
    for (auto new_vertex: candidates) {
        new_clique[new_clique.size() - 1] = new_vertex;

        auto Ns = boost::adjacent_vertices(new_vertex, graph);
        std::vector<V> N_intersection;
        std::set_intersection(Ns.first, Ns.second, candidates.begin(), candidates.end(), std::back_inserter(N_intersection));

        build_k_clique(graph, k, new_clique, N_intersection, cliques);
    }
}

void parallel_process(const Graph& graph, unsigned int k, std::atomic<bool>* mask, std::vector<Clique>& cliques) {
    auto range = boost::edges(graph);
    auto eit = range.first;
    unsigned int edge_position = 0;

    while (eit != range.second) {

        // searching for an edge that hasn't been proceded by another thread
        while(eit != range.second && std::atomic_exchange(mask + edge_position, true)) {
            ++eit;
            ++edge_position;
        }
        if (eit == range.second)
            break;

        V source = boost::source(*eit, graph);
        V target = boost::target(*eit, graph);
        auto Ns = boost::adjacent_vertices(source, graph);
        auto Nt = boost::adjacent_vertices(target, graph);

        std::vector<V> N_intersection;
        std::set_intersection(Ns.first, Ns.second, Nt.first, Nt.second, std::back_inserter(N_intersection));

        build_k_clique(graph, k, {source, target}, N_intersection, cliques);
    }
}

std::vector<std::vector<Clique> > count_cliques(const std::vector<E>& edges_list, unsigned int k, unsigned num_threads = 4) {
    // Input: edges_list -- list of all edges in graph
    //        num_threads - number of threads to multiprocess.

    Graph graph(std::begin(edges_list), std::end(edges_list), 1024);

    // separated vectors for cliques from each thread, since push_back is not thread-safe
    std::vector<std::vector<Clique> > cliques(num_threads);

    // creating array of semaphores, one for each edge
    std::vector<std::atomic<bool>> mask(boost::num_edges(graph));
    for (auto& b : mask)
        std::atomic_init(&b, false);

    std::vector<std::thread> threads;
    for (unsigned i = 0; i < num_threads; ++i) {
        try {
            threads.push_back(std::thread(parallel_process, std::ref(graph), k, &(*mask.begin()), std::ref(cliques[i])));
        } catch (std::exception& e) {
            std::cerr << "Exception caught at opening thread: " << e.what() << '\n';
        }
    }

    for (unsigned i = 0; i < threads.size(); ++i) {
        try {
            threads[i].join();
        } catch (std::exception& e) {
            std::cerr << "Exception caught at closing thread: " << e.what() << '\n';
        }
    }

    return cliques;
}

namespace np = boost::python::numpy;

boost::python::dict clique_enum(boost::python::list edge_list, int max_dim = 3, int workers = -1) {
    if (workers == -1) {
        workers = std::thread::hardware_concurrency();
    }

    int edges_num = boost::python::len(edge_list);
    std::vector<E> edges(edges_num);

    unsigned int left_v, right_v;
    for (int i = 0; i < edges_num; ++i) {
        left_v = boost::python::extract<int>(edge_list[i][0]);
        right_v = boost::python::extract<int>(edge_list[i][1]);

        if (right_v < left_v)
            std::swap(left_v, right_v);
        edges[i] = E(left_v, right_v);
    }
    auto cliques = count_cliques(edges, max_dim, workers);

    std::vector<boost::python::list> tmp(max_dim);

    for (unsigned int i = 0; i < cliques.size(); ++i) {
        for (unsigned int l = 0; l < cliques[i].size(); ++l) {
            int sz = cliques[i][l].size();
            auto x = np::from_data(cliques[i][l].data(), np::dtype::get_builtin<V>(),
                                             boost::python::make_tuple(sz),
                                             boost::python::make_tuple(sizeof(V)),
                                             boost::python::object());
            tmp[sz - 1].append(x.copy());
        }
    }
    boost::python::dict ans;
    for (int i = 3; i <= max_dim; ++i)
        ans[i] = tmp[i - 1];

    return ans;
}

BOOST_PYTHON_FUNCTION_OVERLOADS(foo_overloads, clique_enum, 2, 3)

BOOST_PYTHON_MODULE(CliqueEnumerator) {
    Py_Initialize();
    np::initialize();

    boost::python::def("enumerate_cliques", clique_enum, foo_overloads());
}
