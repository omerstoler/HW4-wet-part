#include <cstdlib>
#include <unistd.h>

Class LogEntry
{
public:
    LogEntry(size_t size,size_t data_size, bool is_free, void* mem_pointer)
    {
      m_size = size;
      m_data_size = data_size;
      m_is_free = is_free;
      m_next=NULL;
      m_mem_pointer = mem_pointer;
    }
    LogEntry* get_next(){return m_next;}
    void set_next(LogEntry* new_next){m_next = new_next;}
    bool is_free(){return m_is_free;}
    void set_free(bool new_status){m_is_free = new_status;}
    size_t get_size(){return m_size;}
    void set_size(size_t new_size){m_size = new_size;}
    size_t get_data_size(){return m_data_size;}
    void set_data_size(size_t new_size){m_data_size = new_size;}
    void* get_mem_pointer(){return m_mem_pointer;}
private:
    LogEntry* m_next;
    bool m_is_free;
    size_t m_size;
    size_t m_data_size;
    void* m_mem_pointer;
}
//======== Log of memory entries =======
LogEntry* log = NULL;

void* malloc(size_t size)
{
  /*
  1. LogEntry is the metadata class
  2. Is there any free available memory (which doesn't require brk)
    2.1 Checked by going over the list and search for pre-allocated areas
    2.2 If there is any, great but don't change its current size
    2.3 If there is no such area, allocate new one using sbrk
  */
  if(size==0 || size>100000000)
  {
    return NULL;
  }

  LogEntry* iter = log;
  LogEntry* temp_entry;

  while (iter != NULL)
  {
    if(iter->is_free() && iter->get_size() >= size)
    {
      iter->set_free(false);
      iter->set_data_size(size);
      return iter->get_mem_pointer();
    }
    iter = iter->next;
  }

  void* prog_brk = sbrk(size+sizeof(LogEntry));

  if(prog_brk==(void*)(-1))
  {
    return NULL;
  }

  temp_entry = (LogEntry*)prog_brk;
  LogEntry log_entry(size,size,false,prog_brk+sizeof(LogEntry)); // Pointers arithmetic
  std::memcpy(temp_entry, &log_entry, sizeof(LogEntry));
  temp_entry->set_next(log);
  log = temp_entry;

  return temp_entry->get_mem_pointer();
}

void* calloc(size_t size)
{
  void* new_mem = malloc(size);
  if(new_mem==NULL)
    return NULL;
  for (size_t i = 0; i < size; i++)
  {
    std::memset(new_mem, 0, size);
  }
  return new_mem;
}

// TODO: Check if needs to prevent allocations at any price
void* realloc(void* oldp, size_t size)
{
  LogEntry* iter = log;
  void* mall_mem;
  size_t old_size=0;

  if(oldp!=NULL)
  {
    while (iter != NULL)
    {
      if(iter->get_mem_pointer() == oldp)
      {
        iter->set_free(true);
        break;
      }
      iter = iter->next;
    }
    old_size = iter->get_data_size();
  }

  mall_mem = malloc(size);

  if(oldp!=NULL)
  {
    if(mall_mem != iter->get_mem_pointer())
    {
      std::memcpy(mall_mem,oldp,old_size);
    }
  }

  return mall_mem;
}

// TODO: check if needed logic to determine if to fold back brk() or not
void free(void* p)
{
  LogEntry* iter = log;

  while (iter != NULL)
  {
    if(iter->get_mem_pointer() == p)
    {
      iter->set_free(true);
    }
    iter = iter->next;
  }
}

size_t _num_free_blocks()
{
  LogEntry* iter = log;
  size_t cnt = 0;
  while (iter != NULL)
  {
    if(iter->is_free())
    {
      cnt++;
      iter = iter->next;
    }
  }
  return cnt;
}

size_t _num_free_bytes()
{
  LogEntry* iter = log;
  size_t sum=0;
  while (iter != NULL)
  {
    if(iter->is_free())
      sum+=iter->get_size();
    iter = iter->next;
  }
  return sum;
}

size_t _num_allocated_blocks()
{
  LogEntry* iter = log;
  size_t count=0;
  while (iter != NULL)
  {
    if(!iter->is_free())
      count++;
    iter = iter->next;
  }
  return count;
}

size_t _num_allocated_bytes()
{

  LogEntry* iter = log;
  size_t sum=0;
  while (iter != NULL)
  {
    if(!iter->is_free())
      sum+=iter->get_size();
    iter = iter->next;
  }
  return sum;
}

size_t _num_meta_data_bytes()
{
  LogEntry* iter = log;
  size_t count=0;
  while (iter != NULL)
  {
    count++;
    iter = iter->next;
  }
  return count*_size_meta_data();
}

size_t _size_meta_data()
{
  return sizeof(LogEntry);
}
