//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Класс вектора, только для структур, 
//| не вызывает конструктор копирования
//+------------------------------------------------------------------+
template <typename T>
class CVector
{
private:
   T                *m_first;
   T                *m_last;
   T                *m_max;

   // для поиска
   struct Lesser
   {
      int operator()(T &left, T &right)
      {
         if(left<right) return(-1);
         if(right<left) return(1);
         return(0);
      }
   };

public:
   //+------------------------------------------------------------------+
   //| Конструкторы
   //+------------------------------------------------------------------+
   CVector()       : m_first(NULL), m_last(NULL), m_max(NULL) {}
   CVector(size_t new_size) : m_first(NULL), m_last(NULL), m_max(NULL) { resize(new_size); }
   //+------------------------------------------------------------------+
   //| Деструктор
   //+------------------------------------------------------------------+
   ~CVector()
   {
      if(m_first!=NULL) delete[] m_first;
      m_first=m_last=m_max=NULL;
   }
   //+------------------------------------------------------------------+
   //| Установка размерности вектора 
   //+------------------------------------------------------------------+
   bool resize(const size_t size)
   {
      if(m_first+size>m_max && !reallocate(size+size/2)) return(false);

      m_last=m_first+size;

      return(true);
   }
   //+------------------------------------------------------------------+
   //| Резервирование памяти                                            |
   //+------------------------------------------------------------------+
   bool reserve(const size_t size)
   {
      //--- резервируем только при нехватке уже выделенной
      return(m_first+size<=m_max || reallocate(size));
   }
   //+------------------------------------------------------------------+
   //| Выделение памяти под вектор с указанием исходного массива        |
   //+------------------------------------------------------------------+
   bool assign(T const * const buf, const size_t size)
   {
      _ASSERT(buf!=NULL || size==0);
      if(buf==NULL && size!=0) return(false);
      // освободим предыдущую память
      if(m_first!=NULL)
      {
         delete[] m_first;
         m_first=m_last=m_max=NULL;
      }
      // выделяем память с коэффициентом 3/2
      if(size && !reallocate(size+size/2)) return(false);
      // копируем буфер
      if(m_first==NULL)
      {
         m_last=NULL;
         m_max =NULL;
      }
      else
      {
         memcpy(m_first, buf, size*sizeof(T));
         // скоректируем максимум элементов
         m_last =m_first+size;
      }
      //---
      return(true);
   }
   //+------------------------------------------------------------------+
   //| Добавление в произвольную позицию массива                        |
   //+------------------------------------------------------------------+
   bool insert(T * const from, T const * const buf, const size_t size)
   {
      _ASSERT(from>=m_first && from<=m_last && (buf!=NULL || size==0));
      if(from<m_first || from>m_last || (buf==NULL && size!=0)) return(false);
      if(buf==NULL || size==0) return(true);

      if(m_last+size>m_max)
      {
         size_t new_size=size+(m_last-m_first);

         size_t calc_size=new_size+new_size/2;

         T* new_buf=new (std::nothrow) T[calc_size];
         if(new_buf==NULL) return(false);

         size_t remain_size=(from-m_first);
         if(m_first!=NULL) memcpy(new_buf, m_first, remain_size*sizeof(T));
         memcpy(new_buf+remain_size, buf, size*sizeof(T));
         if(m_first!=NULL) memcpy(new_buf+remain_size+size, m_first+remain_size, (m_last-from)*sizeof(T));

         if(m_first) delete[] m_first;
         // скорректируем указатели
         m_first=new_buf;
         m_last =new_buf+new_size;
         m_max  =m_first+calc_size;
      }
      else
      {
         memmove(from+size, from, (m_last-from)*sizeof(T));
         memcpy(from, buf, size*sizeof(T));
         m_last+=size;
      }
      //---
      return(true);
   }
   //+------------------------------------------------------------------+
   //| Добавление в произвольную позицию 1-го элемента
   //+------------------------------------------------------------------+
   T* insert(T * const from, const T &elem)
   {
      _ASSERT(from>=m_first && from<=m_last);
      if(from<m_first || from>m_last) return(NULL);

      size_t offset=from-m_first;
      insert(from, &elem, 1);
      return(m_first+offset);
   }
   //+------------------------------------------------------------------+
   //| Возвращает true если контейнер пуст                              |
   //+------------------------------------------------------------------+
   bool              empty() const { return(m_first==m_last); }
   //+------------------------------------------------------------------+
   //| Первый в массиве                                                 |
   //+------------------------------------------------------------------+
   T*                begin() const { return(m_first); }
   //+------------------------------------------------------------------+
   //| Последний в массиве                                              |
   //+------------------------------------------------------------------+
   T*                end() const { return(m_last); }
   //+------------------------------------------------------------------+
   //| Удаление элементов из массива                                    |
   //+------------------------------------------------------------------+
   T*                erase(T* const begin, const T* const end)
   {
      if(begin==end) return(begin);

      _ASSERT(begin>=m_first && begin<m_last && end>m_first && end<=m_last);
      if(begin<m_first || begin>=m_last || end<=m_first || end>m_last) return(NULL);

      memcpy(begin, end, (m_last-end)*sizeof(T));
      m_last-=end-begin;
      return(begin);
   }
   //+------------------------------------------------------------------+
   //| Удаление одного элемента из массива и возврат следующего за ним 
   //+------------------------------------------------------------------+
   T* erase(T* const elem) { return(erase(elem, elem+1)); }
   //+------------------------------------------------------------------+
   //| Получение элемента по индексу
   //+------------------------------------------------------------------+
   T& operator[] (size_t index) const
   {
      assert(index<(size_t)(m_last-m_first));
      return(m_first[index]);
   }
   //+------------------------------------------------------------------+
   //| Получение элемента по индексу
   //+------------------------------------------------------------------+
   T&  At(size_t index) const
   {
      assert(index<(size_t)(m_last-m_first));
      return(m_first[index]);
   }
   //+------------------------------------------------------------------+
   //| Получение указателя на вставку с бинарным поиском
   //| возвращает самый 1-ый в диапазоне который больше или равен 
   //+------------------------------------------------------------------+
   template <typename ST, typename TComparator> T* lowerBound(TComparator comparator, const ST &value)
   {
      T *first=m_first;
      size_t count=m_last-m_first;
      while(count>0)
      {
         size_t half=count/2;
         T *middle=first+half;

         if(comparator(*middle,value)<0)
            first=middle+1, count-=half+1;
         else
            count=half;
      }

      return(first);
   }
   //+------------------------------------------------------------------+
   //| Получение указателя на вставку с бинарным поиском
   //| возвращает самый 1-ый в диапазоне который больше или равен
   //+------------------------------------------------------------------+
   template <typename ST> T* lowerBound(const ST &value)
   {
      T *first=m_first;
      size_t count=m_last-m_first;
      while(count>0)
      {
         size_t half=count/2;
         T *middle=first+half;

         if(*middle<value)
            first=middle+1, count-=half+1;
         else
            count=half;
      }

      return(first);
   }
   //+------------------------------------------------------------------+
   //| Получение указателя на вставку с бинарным поиском 
   //| возвращает самый 1-ый в диапазоне который больше
   //+------------------------------------------------------------------+
   template <typename ST> T* upperBound(const ST &value)
   {
      T *first=m_first;
      size_t count=m_last-m_first;
      while(count>0)
      {
         size_t half=count/2;
         T *middle=first+half;
         //---
         if(!(value<*middle))
            first=middle+1, count-=half+1;
         else
            count=half;
      }
      //---
      return(first);
   }
   //+------------------------------------------------------------------+
   //| Быстрая сортировка всех элементов через предиктор
   //+------------------------------------------------------------------+
   template <typename TComparator>
   void sort(TComparator comparator)
   {
      qSort(comparator, m_first, m_last);
   }
   //+------------------------------------------------------------------+
   //| Быстрая сортировка всех элементов. Требует оператор < 
   //+------------------------------------------------------------------+
   void sort()
   {
      qSort(Lesser(), m_first, m_last);
   }
   //+------------------------------------------------------------------+
   //| Ищет элемент бинарным поиском и возвращает указатель либо NULL
   //+------------------------------------------------------------------+
   template <typename ST> T* binarySearch(const ST &value)
   {
      T* elem=lowerBound(value);

      return(elem==m_last || !(*elem==value) ? NULL : elem);
   }
   //+------------------------------------------------------------------+
   //| Очистка массива (без освобождения, просто исправляем указатели)
   //+------------------------------------------------------------------+
   void              clear() { m_last=m_first; }
   //+------------------------------------------------------------------+
   //| Замена масиивов (просто меняем указатели)
   //+------------------------------------------------------------------+
   void swap(CVector & right)
   {
      T *temp      =right.m_first;
      right.m_first=m_first;
      m_first      =temp;
      //---
      temp         =right.m_last;
      right.m_last =m_last;
      m_last       =temp;
      //---
      temp         =right.m_max;
      right.m_max  =m_max;
      m_max        =temp;
   }
   //+------------------------------------------------------------------+
   //| Размер массива                                                   |
   //+------------------------------------------------------------------+
   size_t            size() const { return(m_last-m_first); }
   //+------------------------------------------------------------------+
   //| Внесение элемента в массив                                       |
   //+------------------------------------------------------------------+
   T* push(const T& elem)
   {
      //--- нужно расширять буфер ?
      if(m_last==m_max)
      {
         size_t cur_size=m_max-m_first;
         size_t allocate=(cur_size>3 ? cur_size+cur_size/2 : cur_size+1);

         if(!reallocate(allocate)) return(NULL);
      }
      //---
      memcpy(m_last, &elem, sizeof(T));

      return(m_last++);
   }
   //+------------------------------------------------------------------+
   //| Внесение пустого элемента в массив и возврат указателя на него   |
   //+------------------------------------------------------------------+
   T* push(void)
   {
      if(m_last==m_max)
      {
         size_t cur_size=m_max-m_first;
         size_t allocate=(cur_size>3 ? cur_size+cur_size/2 : cur_size+1);
         // выделим новый размер с коэффициентом 3/2
         if(!reallocate(allocate)) return(NULL);
      }
      //---
      return(m_last++);
   }
   //+------------------------------------------------------------------+
   //| Убираем послений элемент из массив                               |
   //+------------------------------------------------------------------+
   void pop()
   {
      _ASSERT(m_first<m_last);
      if(m_first>=m_last) return;

      m_last--;
   }
   //+------------------------------------------------------------------+
   //| Оператор присваивания                                            |
   //+------------------------------------------------------------------+
   CVector& operator=(const CVector &other)
   {
      if(&other==this) return(*this);

      assign(other.m_first, other.m_last-other.m_first);

      return(*this);
   }
   //+------------------------------------------------------------------+
   //| Конструктор копий                                                |
   //+------------------------------------------------------------------+
   CVector(const CVector &other) : m_first(NULL), m_last(NULL), m_max(NULL)
   {
      assign(other.m_first, other.m_last-other.m_first);
   }
   //+------------------------------------------------------------------+
   //| Инвертирование вектора                                           |
   //+------------------------------------------------------------------+
   void reverse()
   {
      for(size_t i=0, last=size() - 1, end=size()/2; i < end; ++i)
      {
         T tmp            =m_first[i];
         m_first[i]       =m_first[last - i];
         m_first[last - i]=tmp;
      }
   }

private:
   //+------------------------------------------------------------------+
   //| Внутреннее выделение памяти, size никогда не может быть меньше   |
   //| m_last-m_first                                                   |
   //+------------------------------------------------------------------+
   bool reallocate(const size_t size)
   {

      T* new_buf=new (std::nothrow) T[size];
      if(new_buf==NULL)
      {
         return(false);
      }
      if(m_first)
      {
         memcpy(new_buf, m_first, (m_last-m_first)*sizeof(T));

         delete[] m_first;
      }

      m_last =new_buf+(m_last-m_first);
      m_first=new_buf;
      m_max  =new_buf+size;

      return(true);
   }
   //+------------------------------------------------------------------+
   //| Замена элементов местами (двойной) 
   //+------------------------------------------------------------------+
   void swapDouble(T *elem1, T *elem2)
   {
      if(elem1==elem2) return;

      T tmp;
      memcpy(&tmp,  elem1, sizeof(T));
      memcpy(elem1, elem2, sizeof(T));
      memcpy(elem2, &tmp,  sizeof(T));
   }
   //+------------------------------------------------------------------+
   //| Замена элементов местами (тройной)
   //+------------------------------------------------------------------+
   void swapTriple(T *elem1, T *elem2, T *elem3)
   {
      if(elem1==elem3) return(swapDouble(elem1, elem2));
      if(elem2==elem3) return(swapDouble(elem1, elem3));
      if(elem1==elem2) return(swapDouble(elem2, elem3));

      T tmp;
      memcpy(&tmp,  elem1, sizeof(T));
      memcpy(elem1, elem2, sizeof(T));
      memcpy(elem2, elem3, sizeof(T));
      memcpy(elem3, &tmp,  sizeof(T));
   }
   //+------------------------------------------------------------------+
   //| Быстрая сортировка qsort
   //+------------------------------------------------------------------+
   template <typename TComparator>
   void qSort(TComparator comparator, T *first, T *last)
   {
      if(last-first<=1) return;
      //---
      int    res_lo, res_hi;
      T     *el_lo, *el_hi, *mid_lo, *mid_hi;
      T     *stack_first[sizeof(void*)*8+1];
      T     *stack_last[sizeof(void*)*8+1];
      size_t size_lo, size_hi;
      size_t stack=0;
      // цикл
again:
      el_lo =first;
      el_hi =last-1;
      mid_lo=first+(last-first)/2;
      mid_hi=mid_lo;
      // основной цикл
      do
      {
         res_lo=res_hi=-1;
         // поднимаемся пока слева меньше среднего
         while(el_lo<mid_lo && (res_lo=comparator(*el_lo, *mid_lo))<0) el_lo++;
         // спускаемся пока справа больше среднего
         while(el_hi>mid_hi && (res_hi=comparator(*mid_hi, *el_hi))<0) el_hi--;
         // теперь разбираемся с результатами (9 случаев)
         if(res_lo>0)
         {
            // если все разные, просто меняем
            if(res_hi>0) swapDouble(el_lo++, el_hi--);
            else
            {
               // если правый равен, тройной swap
               swapTriple(el_hi--, el_lo, --mid_lo);
               // правая часть стоит на краю
               if(res_hi<0) mid_hi--;
            }
         }
         else
         {
            // если левый равен, тройной swap
            if(res_hi>0)
            {
               swapTriple(el_lo++, el_hi, ++mid_hi);
               // левая часть стоит на краю
               if(res_lo<0) mid_lo++;
            }
            // просто сдвигаем к центру
            else
            {
               if(res_lo==0) swapDouble(el_lo, --mid_lo);
               if(res_hi==0) swapDouble(el_hi, ++mid_hi);
            }
         }
      }
      while(res_lo!=-1 || res_hi!=-1);
 
      size_lo=mid_lo-first;
      size_hi=last-mid_hi-1;
      // теперь надо выбрать то что помещается в стек а что обрабатывается сразу (большее всегда в стек)
      if(size_lo>=size_hi)
      {
         // сохраняем в стек
         if(size_lo>1)
         {
            stack_first[stack]=first;
            stack_last[stack] =mid_lo;
            stack++;
         }
         // запускаем рекурсию
         if(size_hi>1)
         {
            first=mid_hi+1;
            goto again;
         }
      }
      else
      {
         // сохраняем в стек
         if(size_hi>1)
         {
            stack_first[stack]=mid_hi+1;
            stack_last[stack] =last;
            stack++;
         }
         // запускаем рекурсию
         if(size_lo>1)
         {
            last=mid_lo;
            goto again;
         }
      }
      // раскручиваем стек           
      if(stack>0)
      {
         stack--;
         first=stack_first[stack];
         last =stack_last[stack];
         goto again;
      }
   }
};
//+------------------------------------------------------------------+
