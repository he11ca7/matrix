#include "matrix.h"

// Индексатор по строкам
uint32 indexerRow(uint32 row, uint32 col, uint32, uint32 colCount)
{
  return row * colCount + col;
}
// Индексатор по столбцам
uint32 indexerCol(uint32 row, uint32 col, uint32 rowCount, uint32)
{
  return col * rowCount + row;
}

Matrix::Matrix(uint32 rowCount, uint32 colCount, bool storeRows)
{  
  _rowCount = (rowCount > 0) ? rowCount : 1;
  _colCount = (colCount > 0) ? colCount : 1;
  _storeRows = storeRows;
  _size = _colCount * _rowCount * sizeof(TT);
  _data = (TT *) malloc(_size);

  assert(_data);

  // Включение индексатора
  _indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;

  // Обнуление
  memset((void *) _data, 0, _size);
}

Matrix::~Matrix()
{
  if (!_data) return;
  free((void *) _data);
}

TT &Matrix::v(uint32 row, uint32 col)
{
  return _data[_indexer(row, col, _rowCount, _colCount)];
}

void Matrix::setStoreMode(bool storeRows)
{
  if(_storeRows == storeRows) return;
  _storeRows = storeRows;

  // Временно копировать старые индексатор и данные
  MatrixIndexer indexer = _indexer;
  TT *data = (TT *) malloc(_size);
  assert(_data);
  memcpy(
        (void *) data,
        (void *) _data,
        _size
        );

  // Включить новый индексатор и переместить данные
  _indexer = storeRows ?
        (MatrixIndexer) indexerRow
      :
        (MatrixIndexer) indexerCol;
  for(uint32 i = 0; i < _rowCount; ++i)
    for(uint32 j = 0; j < _colCount; ++j)
      _data[_indexer(i, j, _rowCount, _colCount)] =
          data[indexer(i, j, _rowCount, _colCount)];

  free(data);
}

void Matrix::deleteRow(uint32 row)
{
  if(row >= _rowCount) return;

  _size = _colCount * (_rowCount - 1) * sizeof(TT);

  if(_storeRows)
    {
      // "Сдвинуть" память на место удаляемого блока
      if(row < _rowCount - 1)
        {
          // Позиция "сдвигаемой" части памяти
          uint32 pos = _indexer(row + 1, 0, _rowCount, _colCount);

          memmove(
                (void *) &_data[_indexer(row, 0, _rowCount, _colCount)],
              (void *) &_data[pos],

              (_rowCount * _colCount - pos) * sizeof(TT)
              );
        }
      // Обрезать "хвост" из одной строки
      _data = (TT *) realloc((void *) _data, _size);
      assert(_data);
    }
  else
    {
      TT *data = _data;

      _data = (TT *) malloc(_size);
      assert(_data);

      for(uint32 i = 0; i < _rowCount - 1; ++i)
        for(uint32 j = 0; j < _colCount; ++j)
          _data[_indexer(i, j, _rowCount - 1, _colCount)] =
              data[_indexer(i + ((i >= row) ? 1 : 0),
                            j,
                            _rowCount,
                            _colCount)
              ];
      free(data);
    }

  --_rowCount;
}

// Аналогично deleteRow
void Matrix::deleteCol(uint32 col)
{
  if(col >= _colCount) return;

  _size = (_colCount - 1) * _rowCount * sizeof(TT);

  if(!_storeRows)
    {
      // "Сдвинуть" память на место удаляемого блока
      if(col < _colCount - 1)
        {
          // Позиция "сдвигаемой" части памяти
          uint32 pos = _indexer(0, col + 1, _rowCount, _colCount);

          memmove(
                (void *) &_data[_indexer(0, col, _rowCount, _colCount)],
              (void *) &_data[pos],

              (_rowCount * _colCount - pos) * sizeof(TT)
              );
        }
      // Обрезать "хвост" из одной строки
      _data = (TT *) realloc((void *) _data, _size);
      assert(_data);
    }
  else
    {
      TT *data = _data;

      _data = (TT *) malloc(_size);
      assert(_data);

      for(uint32 i = 0; i < _rowCount; ++i)
        for(uint32 j = 0; j < _colCount - 1; ++j)
          _data[_indexer(i, j, _rowCount, _colCount - 1)] =
              data[_indexer(i,
                            j + ((j >= col) ? 1 : 0),
                            _rowCount,
                            _colCount)
              ];
      free(data);
    }

  --_colCount;
}

void Matrix::resize(uint32 rowCount, uint32 colCount)
{
  if(rowCount == _rowCount && colCount == _colCount) return;

  _size = rowCount * colCount * sizeof(TT);

  // Увеличение числа строк при хранении строками
  // или
  // Увеличение числа столбцов при хранении столбцами
  if(
     (colCount == _colCount && _storeRows)
     ||
     (rowCount == _rowCount && !_storeRows)
     )
    {
      _data = (TT *) realloc((void *) _data, _size);
      assert(_data);

      if(_rowCount < rowCount)
        memset(
              (void *) &_data[_indexer(_rowCount - 1, _colCount - 1, _rowCount, _colCount) + 1],
            0,
            (rowCount * colCount - _rowCount * _colCount) * sizeof(TT)
            );
    }
  // Изменение обеих размерностей
  else
    {
      TT *temp = _data;
      _data = (TT *) malloc(_size);
      assert(_data);

      memset((void *) _data, 0, _size);

      for(uint32 i = 0; i < rowCount; ++i)
        for(uint32 j = 0; j < colCount; ++j)
          if(j < _colCount && i < _rowCount)
            _data[_indexer(i, j, rowCount, colCount)] =
                temp[_indexer(i, j, _rowCount, _colCount)];

      free(temp);
    }

  _colCount = colCount;
  _rowCount = rowCount;
}

TT **Matrix::toPP()
{
  uint32 dimension1 = _storeRows ? _rowCount : _colCount;
  uint32 dimension2 = _storeRows ? _colCount : _rowCount;

  // Память
  TT **result = (TT **) malloc(dimension1 * sizeof(TT *));
  assert(result);
  for(uint32 i = 0; i < dimension1; ++i)
    {
      result[i] = (TT *) malloc(dimension2 * sizeof(TT));
      assert(result[i]);
    }

  // Содержимое
  for(uint32 i = 0; i < _rowCount; ++i)
    for(uint32 j = 0; j < _colCount; ++j)
      result[i][j] = _data[_indexer(i, j, _rowCount, _colCount)];

  return result;
}

void Matrix::printMatrix(Matrix *m)
{
  cout << "Matrix" << endl;
  for(uint32 j = 0; j < m->rowCount(); ++j)
    {
      for(uint32 i = 0; i < m->colCount(); ++i)
        {
          TT value = m->v(j, i);
          cout << setw(5) << value;
        }
      cout << endl;
    }
  cout << endl << endl;
}

void Matrix::printMatrix(TT **m, TT rows, TT cols)
{
  cout << "Matrix" << endl;
  for(uint32 i = 0; i < rows; ++i)
    {
      for(uint32 j = 0; j < cols; ++j)
          cout << setw(10) << m[i][j];
      cout << endl;
    }
  cout << endl << endl;
}