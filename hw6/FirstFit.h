#include <iostream>
#include "AbstractAllocator.h"
#include <cstddef>
#include <stdexcept>
#include <vector>

class FirstFit : public AbstractAllocator
{
public:
    FirstFit() { 
      chunk c(0, poolSize);
      free.push_front(c);
      for (int i = 0; i < poolSize; i ++) {
        pool[i] = '0';
      }
    }

    static bool compLocation(const chunk &c1, const chunk &c2) { 
      return c1.loc < c2.loc; 
    }
    
    void* alloc(size_t bytes) {
      std::list<chunk>::iterator it;
      for (it = free.begin(); it != free.end(); it++) {
        if (it->size >= bytes) {
          std::cout << "Allocating " << bytes << " bytes at memory location " 
               << it->loc << "." << std::endl;
          size_t allocation = it->loc;
          fill(allocation, bytes, true);
          allocated.insert(std::pair<size_t, size_t>(allocation, bytes));
          if (it->size - bytes) {
            // std::cout << "size: " << it->size << " loc: " << it->loc << std::endl;
            it->size = it->size - bytes;
            it->loc = it->loc + bytes;
            // std::cout << "size: " << it->size << " loc: " << it->loc << std::endl;
          } else {
            free.erase(it);
          }
          return &pool[allocation];
        }
      }
      std::cout << "Could not allocate " << bytes << " bytes." << std::endl;
      throw std::bad_alloc();
    }

    void dealloc(void* ptr) {
      size_t index = (char *)ptr - &pool[0];
      size_t bytes = allocated[index];
      std::cout << "Deallocating " << bytes << " bytes at memory location "
                << index << "." << std::endl;
      fill(index, bytes, false);
      std::map<size_t, size_t>::iterator it;
      it = allocated.find(index);
      if (it != allocated.end())
        allocated.erase(it);
      chunk c(index, bytes);
      free.push_back(c);
      free.sort(compLocation);
      merge();
    }

    void merge() {
      std::list<chunk>::iterator outer;
      std::list<chunk>::iterator inner;
      for (outer = free.begin(); outer != free.end(); outer++) {
        for (inner = free.begin(); inner != free.end(); inner++) {
          if ((outer->loc + outer->size) == inner->loc) {           
            outer->size = outer->size + inner->size;      
            free.erase(inner);
            break;
          }
        }
      }
    }

    void fill(size_t loc, size_t bytes, bool alloc) {
      int count = 0;
      for (int i = loc; i < loc + bytes; i ++) {
        pool[i] = alloc ? '1': '0';
        if (alloc) {
          count ++;
        }
      }
      std::cout << "count: " << count << std::endl;
    }

    void print() {
      std::list<chunk>::iterator it;
      std::cout << "[";
      for (it = free.begin(); it != free.end(); it ++) {
        std::cout << "(" << it->loc << ", " << it->size << "),";
      }
      std::cout << "]" << std::endl;
      for (int i = 0; i < poolSize; i++) {
        std::cout << pool[i];
      }
      std::cout << std::endl;
      
    }
};