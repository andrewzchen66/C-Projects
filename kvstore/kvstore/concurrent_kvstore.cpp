#include "concurrent_kvstore.hpp"

#include <optional>

bool ConcurrentKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.shared_mtx.lock_shared();
  // std::shared_lock guard(this->store.shared_mtx);
  std::optional<DbItem> val = this->store.getIfExists(this->store.bucket(req->key), req->key);
  if (val.has_value()) {
    res->value = val.value().value;
    this->store.shared_mtx.unlock_shared();
    return true;
  }
  this->store.shared_mtx.unlock_shared();
  return false;
}

bool ConcurrentKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.shared_mtx.lock();
  this->store.insertItem(this->store.bucket(req->key), req->key, req->value);
  this->store.shared_mtx.unlock();
  return true;
}

bool ConcurrentKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.shared_mtx.lock();
  size_t bucket_num = this->store.bucket(req->key);
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  
  if(val.has_value()) {
    // Append existing value
    this->store.insertItem(bucket_num, req->key, (val.value().value).append(req->value));
    this->store.shared_mtx.unlock();
    return true;
  }
  // Add new value if key not found
  this->store.insertItem(bucket_num, req->key, req->value);
  this->store.shared_mtx.unlock();
  return true;
}

bool ConcurrentKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.shared_mtx.lock();
  size_t bucket_num = this->store.bucket(req->key);
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  if (val.has_value()) {
    res->value = val.value().value;
  }
  bool ret = this->store.removeItem(bucket_num, req->key);
  this->store.shared_mtx.unlock();
  return ret;
}

bool ConcurrentKvStore::MultiGet(const MultiGetRequest* req,
                                 MultiGetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.shared_mtx.lock_shared();
  // std::shared_lock guard(this->store.shared_mtx);
  res->values = {};
  for (std::string key : req->keys) {
    size_t bucket_num = this->store.bucket(key);
    std::optional<DbItem> val = this->store.getIfExists(bucket_num, key);
    if (!val.has_value()) {
      this->store.shared_mtx.unlock_shared();
      return false;
    }
    (res->values).push_back(val.value().value);
  }
  this->store.shared_mtx.unlock_shared();
  return true;
}

bool ConcurrentKvStore::MultiPut(const MultiPutRequest* req,
                                 MultiPutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.shared_mtx.lock();
  if (req->keys.size() != req->values.size()) {
    this->store.shared_mtx.unlock();
    return false;
  }
  for (int i = 0; i < (int) req->keys.size(); i++) {
    this->store.insertItem(this->store.bucket((req->keys)[i]), (req->keys)[i], (req->values)[i]);
  }
  this->store.shared_mtx.unlock();
  return true;
}

std::vector<std::string> ConcurrentKvStore::AllKeys() {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  // std::shared_lock guard(this->store.shared_mtx);
  this->store.shared_mtx.lock_shared();
  std::vector<std::string> keys = {};
  for (std::list<DbItem> bucket : this->store.buckets) {
    for (DbItem item : bucket) {
      keys.push_back(item.key);
    }
  }
  this->store.shared_mtx.unlock_shared();
  return keys;
}
