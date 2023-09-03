#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import requests

if len(sys.argv) == 1:
    print("Usage: {0} city_name [--json|xml]".format(sys.argv[0]))
    exit()

out_format = 'json'
if len(sys.argv) > 2 and sys.argv[2].startswith("--"):
    if sys.argv[2] == "--xml":
        out_format = 'xml'

def query_overpass(query, out_format):
    overpass_url = "http://overpass-api.de/api/interpreter"
    timeout = 25000

    prefix = "[out:{0}][timeout:{1}];\n".format(out_format, timeout)
    query = prefix + query
    headers = {"content-type": "application/x-www-form-urlencoded"}

    r = requests.post(overpass_url, data=query.encode("utf-8"), headers=headers)

    if r.status_code != 200:
        raise Exception("Request failed: HTTP {0}\nText: {1}".format(r.status_code, r.text))

    return r

request_json = query_overpass('area["name"="{0}"]["place"="city"];\nout body;'.format(sys.argv[1]), 'json').json()
area_id = request_json["elements"][0]["id"]

highway_tags = '|'.join([
    "motorway",
    "motorway_link",
    "trunk",
    "trunk_link",
    "primary",
    "primary_link",
    "secondary",
    "secondary_link",
    "tertiary",
    "tertiary_link",
    "unclassified",
    "unclassified_link",
    "residential",
    "residential_link",
    "service",
    "service_link",
    "living_street",
    "pedestrian",
    "road"
])
road_filter = '["highway"~"{0}"]'.format(highway_tags)
query = """
(
 way{0}(area:{1});
 node(w);
);
out body;""".format(road_filter, area_id)

print(query_overpass(query, out_format).text)
