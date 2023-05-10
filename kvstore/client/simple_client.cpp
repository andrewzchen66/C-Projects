#include "simple_client.hpp"

#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

std::optional<std::string> SimpleClient::Get(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  GetRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* get_res = std::get_if<GetResponse>(&*res)) {
    return get_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Get value from server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::Put(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  PutRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* put_res = std::get_if<PutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Put value to server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::Append(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  AppendRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* append_res = std::get_if<AppendResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Append value to server: ", error_res->msg);
  }

  return false;
}

std::optional<std::string> SimpleClient::Delete(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  DeleteRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* delete_res = std::get_if<DeleteResponse>(&*res)) {
    return delete_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Delete value on server: ", error_res->msg);
  }

  return std::nullopt;
}

std::optional<std::vector<std::string>> SimpleClient::MultiGet(
    const std::vector<std::string>& keys) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  MultiGetRequest req{keys};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* multiget_res = std::get_if<MultiGetResponse>(&*res)) {
    return multiget_res->values;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiGet values on server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::MultiPut(const std::vector<std::string>& keys,
                            const std::vector<std::string>& values) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  MultiPutRequest req{keys, values};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* multiput_res = std::get_if<MultiPutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiPut values on server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::GDPRDelete(const std::string& user) {
  // TODO: Write your GDPR deletion code here!
  // You can invoke operations directly on the client object, like so:
  //
  // std::string key("user_1_posts");
  // std::optional<std::string> posts = Get(key);
  // ...
  //
  // Assume the `user` arugment is a user ID such as "user_1".

  // Part 1: Delete all of user's personal posts, and remove user's affiliation with reply posts
  std::string user_posts_key(user + "_posts");
  std::optional<std::string> user_posts_opt = Get(user_posts_key); //Obtain a string of all the user's personal posts
  
  if (user_posts_opt.has_value()) { //if user has any posts
    std::string user_posts = user_posts_opt.value();
    std::istringstream iss_user_posts(user_posts);
    std::string user_post;
    // std::string user_post = strtok(user_posts, ",");
    while (std::getline(iss_user_posts, user_post, ',')) { //For each of the user's personal posts:
      // Get rid of trailing whitespace if it exists
      if (user_post[user_post.length() - 1] == ' ') {
        user_post.erase(user_post.length() - 1, 1);
      }

      //Delete the affiliation between the user's personal posts and their replies, so the replies would essentially be treated as just any other unaffiliated post.  
      // Implement this by deleting the post_id_replies key
      std::string post_replies_key = user_post + "_replies";
      Delete(post_replies_key);

      Delete(user_post); //Delete the actual post
      // user_post = strtok(NULL, ",") // Get next post to delete
    }

    // Update the value of user_id_posts's key-value pair as empty to represent the newly deleted posts of the user. 
    // We aren't deleting the user_id_posts category entirely because the user's account isn't deleted, just all of their posts. User's can choose to delete their account if they wish but that isn't part of GDPRDelete's functionality. 
    Put(user_posts_key, "");
  }
  
  // ----------------------------------------------------------------------------
  // Part 2: Delete all @user tags, username occurences, and hashtags containing the user from other people's posts and replies
  std::string user_tag = "@" + user; //string representing the user tag
  std::string user_name = Get(user).value(); //string representing the username
  user_name.erase(user_name.length() - 1, 1);

  // Convert username to lowercase
  for (char& c : user_name) {
    c = std::tolower(c);
  }

  // Parse the user_name by " " into a vector of individual words
  std::istringstream iss_user_name(user_name);
  std::string user_name_segment;
  std::vector<std::string> parsed_user_name;
  while (std::getline(iss_user_name, user_name_segment, ' ')) {
    // Get rid of trailing whitespace if it exists
    if (user_name_segment[user_name_segment.length() - 1] == ' ') {
      user_name_segment.erase(user_name_segment.length() - 1, 1);
    }
    parsed_user_name.push_back(user_name_segment);
  }

  //Iterate through every user on the platform
  std::istringstream iss_all_users(Get("all_users").value()); //Get string of all user ids on the platform
  std::string some_user_id;
  while (std::getline(iss_all_users, some_user_id, ',')) { 
    // Get rid of trailing whitespace if it exists
    if (some_user_id[some_user_id.length() - 1] == ' ') {
      some_user_id.erase(some_user_id.length() - 1, 1);
    }
    //If the current user has any posts
    if (Get(some_user_id + "_posts").has_value()) { 
      std::istringstream iss_posts_per_user(Get(some_user_id + "_posts").value());
      std::string some_post_id;
      //Iterate through every one of the user's posts
      while(std::getline(iss_posts_per_user, some_post_id, ',')) { 
        // Get rid of trailing whitespace if it exists
        if (some_post_id[some_post_id.length() - 1] == ' ') {
          some_post_id.erase(some_post_id.length() - 1, 1);
        }
        std::string post_content = Get(some_post_id).value(); //Get the post content from kvstore
        
        // replace all instances of "@user_id" tags with ""
        std::string::size_type pos = post_content.find(user_tag);
        while (pos != std::string::npos) {
          post_content.replace(pos, user_tag.length(), "");
          pos = post_content.find(user_tag);
        }

        // Convert post content to lowercase
        std::string lower_post_content = post_content;
        for (char& c : lower_post_content) {
          c = std::tolower(c);
        }

        // Delete all hashtags containing the username from the post
        // First Parse the post_content into vectors of separate words
        std::istringstream iss_post(post_content);
        std::string post_word;
        std::vector<std::string> parsed_post_content;
        while (std::getline(iss_post, post_word, ' ')) {
          parsed_post_content.push_back(post_word);
        }
        // Next parse lower_post_content into vectors of separate words
        std::istringstream iss_lower_post(lower_post_content);
        std::string lower_post_word;
        std::vector<std::string> lower_parsed_post_content;
        while (std::getline(iss_lower_post, lower_post_word, ' ')) {
          lower_parsed_post_content.push_back(lower_post_word);
        }
        post_content = "";
        // Loop through every word in the post
        for (int i = 0; i < (int)parsed_post_content.size(); i++) {
          // Determine if the word is hashtag
          if (lower_parsed_post_content[i][0] == '#') {
            // Loop through every word in the username to check if the part is located in the hashtag
            for (std::string name : parsed_user_name) {
              pos = (lower_parsed_post_content[i]).find(name);
              // Remove the hashtag if contains any part of the name
              if (pos != std::string::npos) {
                lower_parsed_post_content[i] = "";
                parsed_post_content[i] = "";
                // lower_parsed_post_content.erase(lower_parsed_post_content.begin() + i);
                // parsed_post_content.erase(parsed_post_content.begin() + i);
              }
            }
          }
          // Recombine the post vector into one string 
          post_content.append(parsed_post_content[i]);
          post_content += " ";
        }

        // Replace all mentions of the username from the post with "TweetTweet"
        // We only delete mentions of the entire username, e.g. for "matt bleat", instances of "matt" or "bleat" would not be censored.
        // Potential drawback: we don't censor every instance of the username.
        pos = lower_post_content.find(user_name);
        while (pos != std::string::npos) {
          post_content.replace(pos, user_name.length(), "TweetTweet"); //Censor the actual post content 
          lower_post_content.replace(pos, user_name.length(), "TweetTweet"); //Censor the lowercase version of the post content 
          pos = lower_post_content.find(user_name);
        }

        // Put the edited post contents back into the kv store
        Put(some_post_id, post_content);
      }
    }
  }
  return true;
}
