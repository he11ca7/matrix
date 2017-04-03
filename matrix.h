#pragma once

#include "defines.h"

typedef real64 TT;

// TODO Ввести коды ошибки для методов (в виде параметра--ссылки)

/*!
 * \brief Класс Матрица
 *
 * Примитивная реализация двухмерной матрицы.
 */
class Matrix
{
private:

  TT     *_data;
  uint32  _size;          /// Объём выделенной памяти
  uint32
          _rowCount,      /// Количество строк
          _colCount;      /// Количество столбцов
  bool    _storeRows;     /// Признак построчного внутреннего хранения
  TT      _NaN;           /// NaN

  //------------
  // Индексаторы
  //------------

  typedef uint32 (*MatrixIndexer) (uint32 row, uint32 col,
                                   uint32 rowCount, uint32 colCount);
  static uint32 indexerRow(uint32 row, uint32 col, uint32, uint32 colCount);
  static uint32 indexerCol(uint32 row, uint32 col, uint32 rowCount, uint32);
  MatrixIndexer _indexer;

public:

  Matrix(
      uint32 rowCount = 0,
      uint32 colCount = 0,
      bool storeRows = true);
  ~Matrix();

  TT &v(
      uint32 row,
      uint32 col);

  //----------
  // Параметры
  //----------

  uint32 size() const {return _size;}         ///< Объём выделенной памяти
  uint32 rowCount() const {return _rowCount;} ///< Количество строк
  uint32 colCount() const {return _colCount;} ///< Количество столбцов
  bool storeMode() const {return _storeRows;} ///< Способ внутреннего хранения
  void setStoreMode(
      bool storeRows);

  void clear();
  bool isEmpty() const {return _size == 0;}   ///< Является ли матрица пустой

  //-----------
  // Управление
  //-----------

  void deleteRow(
      uint32 row,
      uint32 count = 1);
  void deleteCol(
      uint32 col,
      uint32 count = 1);
  void resize(
      uint32 rowCount,
      uint32 colCount);

  //---------------
  // Преобразование
  //---------------

  TT **toPP();

  //--------
  // Отладка
  //--------

  static void printMatrix(
      Matrix *m,
      int width = 6);

  static void printMatrix(
      TT **m,
      TT rows,
      TT cols,
      int width = 6);
};