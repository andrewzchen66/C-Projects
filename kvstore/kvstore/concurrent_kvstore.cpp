#include "concurrent_kvstore.hpp"

#include <optional>

bool ConcurrentKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  // this->store.shared_mtx.lock_shared();
  size_t bucket_num = this->store.bucket(req->key);
  std::shared_lock guard(this->store.mtx_arr[bucket_num]);
  // this->store.mtx_arr[bucket_num].lock_shared()
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  if (val.has_value()) {
    res->value = val.value().value;
    // this->store.shared_mtx.unlock_shared();
    return true;
  }
  // this->store.shared_mtx.unlock_shared();
  return false;
}

bool ConcurrentKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  size_t bucket_num = this->store.bucket(req->key);
  std::unique_lock guard(this->store.mtx_arr[bucket_num]);
  // this->store.shared_mtx.lock();
  this->store.insertItem(this->store.bucket(req->key), req->key, req->value);
  // this->store.shared_mtx.unlock();
  return true;
}

bool ConcurrentKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  // this->store.shared_mtx.lock();
  size_t bucket_num = this->store.bucket(req->key);
  std::unique_lock guard(this->store.mtx_arr[bucket_num]);
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  
  if(val.has_value()) {
    // Append existing value
    this->store.insertItem(bucket_num, req->key, (val.value().value).append(req->value));
    // this->store.shared_mtx.unlock();
    return true;
  }
  // Add new value if key not found
  this->store.insertItem(bucket_num, req->key, req->value);
  // this->store.shared_mtx.unlock();
  return true;
}

bool ConcurrentKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  // this->store.shared_mtx.lock();
  size_t bucket_num = this->store.bucket(req->key);
  std::unique_lock guard(this->store.mtx_arr[bucket_num]);
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  if (val.has_value()) {
    res->value = val.value().value;
  }
  bool ret = this->store.removeItem(bucket_num, req->key);
  // this->store.shared_mtx.unlock();
  return ret;
}

bool ConcurrentKvStore::MultiGet(const MultiGetRequest* req,
                                 MultiGetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  // this->store.shared_mtx.lock_shared();
  res->values = {};


  std::map<int, size_t> bucket_nums = {};
  std::vector<int> search_count = {};
  for (int i = 0; i < (int) req->keys.size(); i++) {
    bucket_nums[i] = this->store.bucket((req->keys)[i]);
  }
  // Lock all the mutexes in mtx_arr containing a searched key
  for (int i = 0; i < (int)this->store.BUCKET_COUNT; i++) {
    search_count.push_back(bucket_nums.count(i));
    if (search_count[i] > 0) {
      this->store.mtx_arr[i].shared_lock();
    }
  }





  for (int i = 0; i < (int) req->keys.size(); i++) {
    // size_t bucket_num = this->store.bucket(key);
    // std::shared_lock guard(this->store.mtx_arr[bucket_num]);
    std::optional<DbItem> val = this->store.getIfExists(bucket_nums[i], req->keys[i]);
    if (!val.has_value()) {
      // this->store.shared_mtx.unlock_shared();
      for (int i = 0; i < (int)this->store.BUCKET_COUNT; i++) {
        if (search_count[i] > 0) {
          this->store.mtx_arr[i].shared_unlock();
        }
      }
      return false;
    }
    (res->values).push_back(val.value().value);
    search_count[bucket_nums[i]]--;
    if (search_count[bucket_nums[i]] == 0) {
      this->store.mtx_arr[bucket_nums[i]].shared_unlock();
    }
  }
  // this->store.shared_mtx.unlock_shared();
  return true;
}

bool ConcurrentKvStore::MultiPut(const MultiPutRequest* req,
                                 MultiPutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  // this->store.shared_mtx.lock();
  if (req->keys.size() != req->values.size()) {
    // this->store.shared_mtx.unlock();
    return false;
  }
  std::map<int, size_t> bucket_nums = {};
  std::vector<int> search_count = {};
  for (int i = 0; i < (int) req->keys.size(); i++) {
    bucket_nums[i] = this->store.bucket((req->keys)[i]);
  }
  // Lock all the mutexes in mtx_arr containing a searched key
  for (int i = 0; i < (int)this->store.BUCKET_COUNT; i++) {
    search_count.push_back(bucket_nums.count(i));
    if (search_count[i] > 0) {
      this->store.mtx_arr[i].lock();
    }
  }
  // Perform the multiput, unlocking once the bucket no longer needs to be used
  for (int i = 0; i < (int) req->keys.size(); i++) {
    // std::unique_lock guard(this->store.mtx_arr[bucket_num]);
    this->store.insertItem(bucket_nums[i], (req->keys)[i], (req->values)[i]);
    search_count[bucket_nums[i]]--;
    if (search_count[bucket_nums[i]] == 0) {
      this->store.mtx_arr[bucket_nums[i]].unlock();
    }
  }
  // this->store.shared_mtx.unlock();
  return true;
}

std::vector<std::string> ConcurrentKvStore::AllKeys() {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::vector<std::string> keys = {};
  // Lock all the mutexes in mtx_arr;
  for (int bucket_num = 0; bucket_num < (int)this->store.BUCKET_COUNT; bucket_num++) {
    this->store.mtx_arr[bucket_num].lock();
  }
  // Add keys, unlocking mutexes as you go;
  for (int bucket_num = 0; bucket_num < (int)this->store.BUCKET_COUNT; bucket_num++) {
    for (DbItem item : this->store.buckets[bucket_num]) {
      keys.push_back(item.key);
    }
    this->store.mtx_arr[bucket_num].unlock();
  }
  return keys;
}
