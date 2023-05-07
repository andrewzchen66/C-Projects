#include "simple_kvstore.hpp"

bool SimpleKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  for (std::map<std::string, std::string>::iterator it = kv_map.begin(); it != kv_map.end(); ++it) {
    if (it->first == req->key) {
      res->value = kv_map[req->key];
      mtx.unlock();
      return true;
    }
  }
  mtx.unlock();
  return false;
}

bool SimpleKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  kv_map[req->key] = req->value;
  mtx.unlock();
  return true;
}

bool SimpleKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  std::unique_lock guard(mtx);
  if (kv_map.count() > 0) {
    (kv_map[req->key]).append(req->value);
    return true;
  }
  kv_map[req->key] = req->value;
  return true;


  // for (std::map<std::string, std::string>::iterator it = kv_map.begin(); it != kv_map.end(); ++it) {
  //   // append to existing value if found
  //   if (it->first == req->key) {
  //     (kv_map[req->key]).append(req->value);
  //     // std::string combined_str;
  //     // combined_str.append(kv_map[req->key]);
  //     // combined_str.append(req->value);
  //     // kv_map[req->key] = combined_str;
  //     // mtx.unlock();
  //     return true;
  //   }
  // }
  // // Create new kv pair
  // kv_map[req->key] = req->value;
  // // mtx.unlock();
  // return true;
}

bool SimpleKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // successfully gets the value
  mtx.lock();
  for (std::map<std::string, std::string>::iterator it = kv_map.begin(); it != kv_map.end(); ++it) {
    // store value, delete entry
    if(it->first == req->key) {
      res->value = kv_map[req->key];
      kv_map.erase(req->key);
      mtx.unlock();
      return true;
    }
  }
  // key doesn't exist
  mtx.unlock();
  return false;
}

bool SimpleKvStore::MultiGet(const MultiGetRequest* req,
                             MultiGetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  res->values = {};
  for (std::string key : req->keys) {
    bool found_key = false;
    for (std::map<std::string, std::string>::iterator it = kv_map.begin(); it != kv_map.end(); ++it) {
      // store value in res
      if(it->first == key) {
        found_key = true;
        (res->values).push_back(kv_map[key]);
      }
    }
    // Key not found yet
    if (!found_key) {
      mtx.unlock();
      return false;
    }
  }
  mtx.unlock();
  return true;
}

bool SimpleKvStore::MultiPut(const MultiPutRequest* req, MultiPutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  if (req->keys.size() != req->values.size()) {
    mtx.unlock();
    return false;
  }
  for (int i = 0; i < (int) req->keys.size(); i++) {
    kv_map[(req->keys)[i]] = (req->values)[i];
  }
  mtx.unlock();
  return true;
}

std::vector<std::string> SimpleKvStore::AllKeys() {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  std::vector<std::string> keys = {};
  for (std::map<std::string, std::string>::iterator it = kv_map.begin(); it != kv_map.end(); ++it) {
    keys.push_back(it->first);
  }
  mtx.unlock();
  return keys;
}
