#pragma once

#include "defines.h"

typedef real64 TT; // Для данных
typedef uint32 TI; // Для итераторов

// TODO Коды ошибки для методов (в виде параметра--ссылки)
// TODO Ввести методы получения отдельных строк/столбцов (нескольких подряд)
// TODO Свести в одну _defaultValue и _NaN [в _defaultValue]

/*!
 * \brief Класс Матрица
 *
 * Реализация двухмерной матрицы.
 */
class Matrix
{
private:

  TT     *_data;
  TI      _size;          // Объём выделенной памяти
  TI
          _rowCount,      // Количество строк
          _colCount;      // Количество столбцов
  bool    _storeRows;     // Признак построчного внутреннего хранения
  TT      _defaultValue;  // Значение по умолчанию
  TT      _NaN;           // NaN

  //------------
  // Индексаторы
  //------------

  typedef TI (*MatrixIndexer) (TI row, TI col, TI rowCount, TI colCount);
  static TI indexerRow(TI row, TI col, TI, TI colCount);
  static TI indexerCol(TI row, TI col, TI rowCount, TI);
  MatrixIndexer _indexer;

public:

  Matrix(
      TI rowCount,
      TI colCount,
      bool storeRows = true);
  Matrix(
      bool storeRows = true) : Matrix(0, 0, storeRows) {} // NOTE C++11
  ~Matrix();

  //----------------
  // Доступ к данным
  //----------------

  TT &o(
      TI row,
      TI col);

  //----------
  // Параметры
  //----------

  TI size() const {return _size;}                 ///< Объём выделенной памяти
  TI rowCount() const {return _rowCount;}         ///< Количество строк
  TI colCount() const {return _colCount;}         ///< Количество столбцов
  bool storeMode() const {return _storeRows;}     ///< Способ внутреннего хранения
  TT defaultValue() const {return _defaultValue;} ///< Значение по умолчанию

  void setStoreMode(
      bool storeRows);
  void setDefaultValue(
      TT defaultValue) {_defaultValue = defaultValue;}

  void clear();
  bool isEmpty() const {return _size == 0;} ///< Является ли матрица пустой

  //-----------
  // Управление
  //-----------

  void deleteRow(
      TI row,
      TI count = 1);
  void deleteCol(
      TI col,
      TI count = 1);
  void resize(
      TI rowCount,
      TI colCount);
  void setRowCount(
      TI rowCount);
  void setColCount(
      TI colCount);

  //---------------
  // Преобразование
  //---------------

  TT **toPP();
  TT *toP();

  static Matrix *fromPP(
      TT **PP,
      TI rowCount,
      TI colCount,
      bool storeRows = true);
  static Matrix *fromP(
      TT *P,
      TI rowCount,
      TI colCount,
      bool storeRows = true);

  //--------
  // Отладка
  //--------

  static void printMatrix(
      Matrix *m,
      int width = 6);

  static void printMatrix(
      TT **m,
      TI rows,
      TI cols,
      int width = 6);
};
