from distutils.core import setup
from distutils.extension import Extension

CliqueEnumerator = Extension(
    'CliqueEnumerator',
    sources=['CliqueEnumerator.cpp'],
    libraries=['boost_python3', 'boost_numpy3']
)

setup(
    name='CliqueEnumerator',
    version='0.1',
    ext_modules=[CliqueEnumerator])
