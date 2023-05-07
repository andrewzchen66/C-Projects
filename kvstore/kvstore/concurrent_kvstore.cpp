#include "concurrent_kvstore.hpp"

#include <optional>

bool ConcurrentKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::optional<DbItem> val = this->store.getIfExists(this->store.bucket(req->key), req->key);
  if (val.has_value()) {
    res->value = val.value().value;
    return true;
  }
  return false;
}

bool ConcurrentKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  this->store.insertItem(this->store.bucket(req->key), req->key, req->value);
  return true;
}

bool ConcurrentKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  size_t bucket_num = this->store.bucket(req->key);
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  
  if(val.has_value()) {
    // Append existing value
    this->store.insertItem(bucket_num, req->key, (val.value().value).append(req->value));
    return true;
  }
  // Add new value if key not found
  this->store.insertItem(bucket_num, req->key, req->value);
  return true;
}

bool ConcurrentKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  size_t bucket_num = this->store.bucket(req->key);
  std::optional<DbItem> val = this->store.getIfExists(bucket_num, req->key);
  if (val.has_value()) {
    res->value = val.value().value;
  }
  return this->store.removeItem(bucket_num, req->key);
  
  // res->value = getIfExists(bucket_num, req->key).value;
  // if(!(res->value)) {
  //   return false
  // }
  // remove
  // return true;
}

bool ConcurrentKvStore::MultiGet(const MultiGetRequest* req,
                                 MultiGetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  res->values = {};
  for (std::string key : req->keys) {
    size_t bucket_num = this->store.bucket(key);
    std::optional<DbItem> val = this->store.getIfExists(bucket_num, key);
    if (!val.has_value()) {
      return false;
    }
    (res->values).push_back(val.value().value);
  }
  return true;
}

bool ConcurrentKvStore::MultiPut(const MultiPutRequest* req,
                                 MultiPutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  if (req->keys.size() != req->values.size()) {
    return false;
  }
  for (int i = 0; i < (int) req->keys.size(); i++) {
    this->store.insertItem(this->store.bucket((req->keys)[i]), (req->keys)[i], (req->values)[i]);
  }
  return true;
}

std::vector<std::string> ConcurrentKvStore::AllKeys() {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::vector<std::string> keys = {};
  for (std::list<DbItem> bucket : this->store.buckets) {
    for (DbItem item : bucket) {
      keys.push_back(item.key);
    }
  }
  return keys;
}
