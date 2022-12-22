#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#define BUFFER_SIZE 32768
#ifdef DEBUG
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...)
#endif

struct buffer
{
  char *data;
  size_t size;
};

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  struct buffer *buffer = (struct buffer *)userdata;
  size_t new_size = buffer->size + size * nmemb;
  buffer->data = (char *)realloc(buffer->data, new_size + 1);
  if (buffer->data == NULL)
  {
    fprintf(stderr, "Failed to reallocate memory for buffer\n");
    return 0;
  }
  memcpy(buffer->data + buffer->size, ptr, size * nmemb);
  buffer->data[new_size] = '\0';
  buffer->size = new_size;
  return size * nmemb;
}

int main(int argc, char *argv[])
{
  dprintf("altstore-beta source generator DEBUG build\n");
  if (argc < 7)
  {
    fprintf(stderr, "usage: %s <apps.json url> <news.json location> <new apps.json location> <safe apps.json location> <repository url> <safe repository url>\n", argv[0]);
    return 1;
  }

  // Read news.json
  json_t *news;
  json_error_t error;
  news = json_load_file(argv[2], 0, &error);
  if (!news)
  {
    fprintf(stderr, "Failed to load news.json: %s at line %d column %d\n", error.text, error.line, error.column);
    return 1;
  }

  // Download apps.json
  dprintf("Downloading JSON\n");
  CURL *curl;
  CURLcode res;
  struct buffer buffer = {NULL, 0};
  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "AltStore/1 CFNetwork/1331.0.7 Darwin/21.4.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  dprintf("Downloaded JSON\n");

  // Parse apps.json
  json_t *appsjson;
  appsjson = json_loads(buffer.data, 0, &error);
  if (!appsjson)
  {
    fprintf(stderr, "Failed to parse apps.json: %s at line %d column %d\n", error.text, error.line, error.column);
    return 1;
  }
  // Add news to apps.json
  json_object_set(appsjson, "news", news);

  // Modify apps
  json_t *beta_apps = json_array();
  json_t *apps = json_object_get(appsjson, "apps");
  size_t index;
  json_t *app;
  json_array_foreach(apps, index, app)
  {
    int beta = json_is_true(json_object_get(app, "beta"));
#ifdef DEBUG
    const char* app_name = json_string_value(json_object_get(app, "name"));
    dprintf("App %s beta status %d\n", app_name, beta);
#endif
    if (beta)
    {
      dprintf("Beta app %s found! Appending.\n", app_name);
      json_object_set(app, "beta", json_false());
      const char *name = json_string_value(json_object_get(app, "name"));
      char *name_beta = malloc(strlen(name) + 8); // 8 = strlen(" [Beta]")
      strcpy(name_beta, name);
      strcat(name_beta, " [Beta]");
      json_object_set(app, "name", json_string(name_beta));
      json_array_append(beta_apps, app);
      free(name_beta);
    }
  }
  json_object_set(appsjson, "apps", beta_apps);

  // Set other fields in apps.json
  json_object_set(appsjson, "sourceURL", json_string(argv[5]));
  json_t *user_info = json_object();
  json_object_set(appsjson, "userInfo", user_info);
  json_object_set(appsjson, "identifier", json_string("lol.nickchan.altstore-beta"));
  json_object_set(appsjson, "name", json_string("AltStore Beta"));

  // Write out modified apps.json
  dprintf("Writing modified JSON to %s\n", argv[3]);
  json_dump_file(appsjson, argv[3], JSON_INDENT(2));

  // Modify apps.json again for safe version
  json_object_set(appsjson, "name", json_string("AltStore Beta (Trusted bypass)"));
  json_object_set(appsjson, "identifier", json_string("io.altstore.example")); // pretend to be a hidden trusted source
  json_object_set(appsjson, "sourceURL", json_string(argv[6]));

  // Write out safe version of apps.json
  dprintf("Writing bypass JSON to %s\n", argv[4]);
  json_dump_file(appsjson, argv[4], JSON_INDENT(2));

  // Clean up
  json_decref(appsjson);
  json_decref(news);
  free(buffer.data);

  return 0;
}
