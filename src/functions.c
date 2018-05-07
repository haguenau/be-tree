#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "betree.h"
#include "functions.h"

bool within_frequency_caps(const struct frequency_caps_list *caps, enum frequency_type_e type, uint32_t id, const struct string_value namespace, uint32_t value, size_t length, int64_t now) 
{
    for (size_t i = 0; i < caps->size; i++) {
        if (caps->content[i].id == id && caps->content[i].namespace.str == namespace.str && caps->content[i].type == type) {
            if (length <= 0) {
                return value > caps->content[i].value;
            }
            else if (!caps->content[i].timestamp_defined) {
                return true;
            }
            else if ((now - (caps->content[i].timestamp / 1000000)) > length) {
                return true;
            }
            else if (value > caps->content[i].value) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    return true;
}

bool segment_within(int64_t segment_id, int32_t after_seconds, const struct segments_list *segments, int64_t now) 
{
    for(size_t i = 0; i < segments->size; i++) {
        if(segments->content[i].id < segment_id) {
            continue;
        }
        else if(segments->content[i].id == segment_id) {
            return (now - after_seconds) <= (segments->content[i].timestamp / 1000000);
        }
        else {
            return false;
        }
    }
    return false;
}

bool segment_before(int64_t segment_id, int32_t before_seconds, const struct segments_list *segments, int64_t now) 
{
    for (size_t i = 0; i < segments->size; i++) {
        if (segments->content[i].id < segment_id) {
            continue;
        }
        else if (segments->content[i].id == segment_id) {
            return (now - before_seconds) > (segments->content[i].timestamp / 1000000);
        }
        else {
            return false;
        }
    }
    return false;
}

#define EARTH_RADIUS 6372.8
#define TO_RAD (3.1415926536 / 180)

bool geo_within_radius(double lat1, double lon1, double lat2, double lon2, double distance)
{
    double dx, dy, dz;
    lon1 -= lon2;
    lon1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;

    dz = sin(lat1) - sin(lat2);
    dx = cos(lon1) * cos(lat1) - cos(lat2);
    dy = sin(lon1) * cos(lat1);

    return (asin(sqrt(dx*dx + dy*dy + dz*dz) / 2) * 2 * EARTH_RADIUS) <= distance;
}

bool contains(const char* value, const char* pattern)
{
    size_t value_size = strlen(value);
    size_t pattern_size = strlen (pattern);
	if (value_size < pattern_size) return false;

	return strstr(value, pattern) != NULL;
}

bool starts_with(const char* value, const char* pattern) 
{
    size_t value_size = strlen(value);
    size_t pattern_size = strlen (pattern);
	if (value_size < pattern_size) return false;

	return strstr(value, pattern) != NULL;
}

bool ends_with(const char* value, const char* pattern)
{
    size_t value_size = strlen(value);
    size_t pattern_size = strlen (pattern);
	if (value_size < pattern_size) return false;

	size_t off = value_size - pattern_size;
	return strstr(value + off, pattern) != NULL;
}
