// Copyright 2009, Google Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of Google Inc. nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// This lists a users Google My Maps using the Google Maps Data API.  The
// list of maps is an Atom feed where each entry is a Google My Map.

#include <iostream>
#include <string>
#include "boost/scoped_ptr.hpp"
#include "curlfetch.h"
#include "prompt.h"
#include "kml/base/file.h"
#include "kml/dom.h"
#include "kml/convenience/http_client.h"
#include "kml/convenience/google_maps_data.h"

using kmlconvenience::GoogleMapsData;

int main(int argc, char** argv) {
  std::string user;
  std::string password;

  std::cout << "Email: ";
  std::cin >> user;
  password = Prompt::PromptAndInputWithNoEcho("Password: ");

  CurlHttpClient* curl_http_client =
      new CurlHttpClient("libkml:examples:hellonet:listgmaps");
  if (!curl_http_client->Login(GoogleMapsData::get_service_name(), user,
                               password)) {
    std::cerr << "Login failed" << std::endl;
    return 1;
  }

  boost::scoped_ptr<GoogleMapsData> google_maps_data(
      GoogleMapsData::Create(curl_http_client));

  kmldom::AtomFeedPtr meta_feed = google_maps_data->GetMetaFeed();
  if (!meta_feed.get()) {
    std::cerr << "GetMetaFeed failed" << std::endl;
  }

  // For each <entry> in the <feed>...
  for (size_t e = 0; e < meta_feed->get_entry_array_size(); ++e) {
    const kmldom::AtomEntryPtr& entry = meta_feed->get_entry_array_at(e);
    // Print the <title>:
    std::cout << "[title] " << entry->get_title() << std::endl;
    std::string feature_feed_uri;
    if (GoogleMapsData::GetFeatureFeedUri(entry, &feature_feed_uri)) {
      std::cout << " [feature feed] " << feature_feed_uri << std::endl;
    }
    for (size_t l = 0; l < entry->get_link_array_size(); ++l) {
      // For each <link> in the entry...
      const kmldom::AtomLinkPtr& link = entry->get_link_array_at(l);
      // Print the rel= and href=.
      std::cout << " [rel ] " << link->get_rel() << std::endl;
      std::cout << " [href] " << link->get_href() << std::endl;
    }
  }

  std::cout << "You have " << meta_feed->get_entry_array_size() << " maps.";
  std::cout << std::endl;

  return 0;
}
