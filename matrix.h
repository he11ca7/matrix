#pragma once

#include <iostream>
#include <iomanip>
using namespace std;

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "defines.h"

typedef uint16 TT; // real32

typedef uint32 (*MatrixIndexer) (uint32 row, uint32 col,
                                 uint32 rowCount, uint32 colCount);

/*!
   \brief Класс Матрица
 */
class Matrix
{
public:

  TT      *_data;
  uint32  _size;      /// Объём выделенной памяти
  uint32
          _rowCount,  /// Количество строк
          _colCount;  /// Количество столбцов
  bool    _storeRows; /// Признак построчного внутреннего хранения

  MatrixIndexer _indexer; /// Индексатор

public:

  Matrix(uint32 rowCount = 1, uint32 colCount = 1, bool storeRows = true);
  ~Matrix();

  TT &v(uint32 row, uint32 col);

  uint32 rowCount() const {return _rowCount;}
  uint32 colCount() const {return _colCount;}

  bool storeMode() const {return _storeRows;}
  void setStoreMode(bool storeRows);

  void deleteRow(uint32 row);
  void deleteCol(uint32 col);

  void resize(uint32 rowCount, uint32 colCount);

  TT **toPP();

  // DEBUG
  static void printMatrix(Matrix *m);
  static void printMatrix(TT **m, TT rows, TT cols);
};