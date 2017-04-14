#pragma once

#include "defines.h"

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
public:

  typedef real64 TT; // Данные
  typedef uint32 TI; // Итераторы

  static TI indexerRow(TI row, TI col, TI, TI colCount);
  static TI indexerCol(TI row, TI col, TI rowCount, TI);

private:

  typedef TI (*MatrixIndexer) (TI row, TI col, TI rowCount, TI colCount);

  TI
  _rowCount,  // Число строк
  _colCount,  // Число столбцов
  _size;      // Объём памяти данных

  TT
  *_data,         // Данные
  _defaultValue,  // Значение по умолчанию для memset
  _NaN;           // NaN

  bool _storeRows;         // Признак построчного внутреннего хранения
  MatrixIndexer _indexer;  // Индексатор

public:

  Matrix(
      TI rowCount,
      TI colCount,
      bool storeRows = true);
  Matrix(
      bool storeRows = true) : Matrix(0, 0, storeRows) {} // NOTE C++11
  Matrix(
      const Matrix &copy);
  ~Matrix();

  Matrix &operator=(
      const Matrix &copy);
  bool operator==(
      const Matrix &other);

  //----------------
  // Доступ к данным
  //----------------

  TT &o(
      TI row,
      TI col);
  const TT &o(
      TI row,
      TI col) const;

  //----------
  // Параметры
  //----------

  TI size() const {return _size;}                 ///< Объём выделенной памяти
  TI rowCount() const {return _rowCount;}         ///< Количество строк
  TI colCount() const {return _colCount;}         ///< Количество столбцов
  bool storeMode() const {return _storeRows;}     ///< Способ внутреннего хранения

  void setStoreMode(
      bool storeRows);
  void setDefaultValue(
      TT defaultValue) {_defaultValue = defaultValue;}

  void clear();
  bool isEmpty() const {return _size == 0;} ///< Является ли матрица пустой

  //-----------
  // Управление
  //-----------

  Matrix *part(
      TI rowBeg,
      TI rowEnd,
      TI colBeg,
      TI colEnd,
      bool copy = false);
  void resize(
      TI rowCount,
      TI colCount);
  void deleteRow(
      TI row,
      TI count = 1);
  void deleteCol(
      TI col,
      TI count = 1);
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
      bool storeRows = true,
      int width = 6);
};
