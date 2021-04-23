# CliqueEnumerator
Python+Boost implementation of parallel clique enumerator. 

### Building

Для сборки библиотеки необходим python3, библиотеки Boost-Python и Boost-Numpy выполнить
```
$ python3 setup.py build_ext --inplace
```
Полученный .so файл и будет подключаемой библиотекой.

### Usage
Пример использования - в файле task1.py
```
import CliqueEnumerator
```
Формат входных данных: список рёбер в графе, максимальный размер клик (>= 3), опционально --- количество потоков (по умолчанию вычисления распараллеливаются на все доступные cpu).
```
>>> edge_list = [[1,2], [3,4], ..... ]
>>> max_size = 5
>>> num_workers = 4
```

Вычисление
```
>>> results = CliqueEnumerator.enumerate_cliques(edge_list, max_size, num_workers)

OR

>>> results = CliqueEnumerator.enumerate_cliques(edge_list, max_size)
```

Формат выходных данных:
Словарь {размер---список клик данного размера}. Выводятся только клики размеров 3,...,  max_size. Каждая клика представлена в виде NumPy array.
```
>>> results
{3: [array([1, 2, 3], dtype=int32), array([1, 2, 5], dtype=int32), array([1, 3, 5], dtype=int32), array([1, 4, 6], dtype=int32), array([2, 3, 5], dtype=int32)], 4: [array([1, 2, 3, 5], dtype=int32)]}

```
