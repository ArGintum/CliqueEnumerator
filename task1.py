import CliqueEnumerator

a = [[1,2], [1,3], [1,4], [2,3], [1,5], [2,5], [3,5], [1,6], [4,6]]

b = CliqueEnumerator.enumerate_cliques(a, 4)
print(b)
