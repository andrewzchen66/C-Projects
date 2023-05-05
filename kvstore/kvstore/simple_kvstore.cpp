#include "simple_kvstore.hpp"

bool SimpleKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // if ((req->key != NULL)) {
  //   return false;
  // }
  std::vector<std::string> keys = AllKeys();
  for (int i = 0; i < (int) keys.size(); i++) {
    if (keys[i] == req->key) {
      res->value = kv_map[req->key];
      return true;
    }
  }
  return false;
}

bool SimpleKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // if (!(req->key) || !(req->value)) {
  //   return false;
  // }
  kv_map[req->key] = req->value;
  return true;
}

bool SimpleKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // if (!(req->key) || !(req->value)) {
  //   return false;
  // }
  GetRequest* get_req = new GetRequest;
  get_req->key = req->key;
  GetResponse* get_res = new GetResponse;
  // Append to existing value
  if (Get(get_req, get_res)) {
    std::string combined_str;
    combined_str.append(get_res->value);
    combined_str.append(req->value);
    kv_map[req->key] = combined_str;
    delete get_req;
    delete get_res;
    return true;
  }
  // Create new value
  else {
    kv_map[req->key] = req->value;
    delete get_req;
    delete get_res;
    return true;
  }
}

bool SimpleKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // if (!(req->key)) {
  //   return false;
  // }
  GetRequest* get_req = new GetRequest;
  get_req->key = req->key;
  GetResponse* get_res = new GetResponse;
  // successfully gets the value
  if(Get(get_req, get_res)) {
    std::string result = get_res->value;
    res->value = result;
    kv_map.erase(req->key);
    delete get_req;
    delete get_res;
    return true;
  }
  // key doesn't exist
  delete get_req;
  delete get_res;
  return false;
}

bool SimpleKvStore::MultiGet(const MultiGetRequest* req,
                             MultiGetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // if (!(req->keys)) {
  //   return false;
  // }
  GetRequest* get_req = new GetRequest;
  GetResponse* get_res = new GetResponse;
  for (std::string key : req->keys) {
    get_req->key = key;
    // Key not found
    if(!Get(get_req, get_res)) {
      delete get_req;
      delete get_res;
      return false;
    }
    // Add value 
    (res->values).push_back(get_res->value);
  }
  delete get_req;
  delete get_res;
  return true;
}

bool SimpleKvStore::MultiPut(const MultiPutRequest* req, MultiPutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // if (!(req->keys) || !(req->values) || (req->keys.size() != req->values.size())) {
  //   return false;
  // }
  if (req->keys.size() != req->values.size()) {
    return false;
  }
  PutRequest* put_req = new PutRequest;
  PutResponse* put_res = new PutResponse;
  for (int i = 0; i < (int)req->keys.size(); i++) {
    put_req->key = (req->keys)[i];
    put_req->value = (req->values)[i];
    if (!Put(put_req, put_res)) {
      delete put_req;
      delete put_res;
      return false;
    }
  }
  delete put_req;
  delete put_res;
  return true;
}

std::vector<std::string> SimpleKvStore::AllKeys() {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  std::vector<std::string> keys = {};
  for (const std::pair<std::string, std::string>& kv : kv_map) {
    keys.push_back(kv.first);
  }
  return keys;
}
