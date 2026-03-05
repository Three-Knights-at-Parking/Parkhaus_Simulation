#include <stdio.h>
#include <json-c/json.h>

int main() {
    json_object *obj = json_object_new_object();
    json_object_object_add(obj, "message", json_object_new_string("Hello JSON World!"));
    printf("%s\n", json_object_to_json_string(obj));
    return 0;
}