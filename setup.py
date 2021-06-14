from setuptools import setup, find_packages, Extension

setup(
    name="mykmeanssp",
    version="0.1.0",
    author="eden and noa",
    author_email="edentsubery@mail.tau.ac.il",
    description="a sample C-API",
    install_requires=['invoke'],
    packages=find_packages(),
    license='GPL-2',


    classifiers=[
        'Development Status:: 3- Alpha',
        'License:: OSI Approved:: GNU General Public License v2 (GPLv2)',
        'Natural Language:: English',
        'Programming Language:: Python :: 3:: Only',
        'Programming Language:: Python :: Implementation :: CPython',
        ],
    
    ext_modules=[
        Extension(
            'mykmeanssp',
            ['kmeans.c'],
        ),
    ],
)