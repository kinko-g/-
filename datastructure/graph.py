

# graph = {
#     'a':{'b','c'},
#     'b':{'a','c','d'},
#     'c':{'a','b','d','e'},
#     'd':{'b','c','e','f'},
#     'e':{'c','d'},
#     'f':{'d'}
# }

# def dfs():
#     s = ['e']
#     visited = {'e'}
#     parent = {'e':None}

#     while len(s) > 0:
#         root = s.pop(0)
#         print(root,end='\t')
#         for c in graph[root]:
#             if c not in visited:
#                 parent[c] = root
#                 visited.add(c)
#                 s.append(c)
#     return parent

# shortest_path = dfs()
# print()
# vertex = 'b'
# while vertex:
#     print(vertex,end='\t')
#     vertex = shortest_path[vertex]




#################################################

import heapq
import math

graph = {
    'a':{'b':5,'c':1},
    'b':{'a':5,'c':2,'d':1},
    'c':{'a':1,'b':2,'d':4,'e':8},
    'd':{'b':1,'c':4,'e':3,'f':6},
    'e':{'c':8,'d':3},
    'f':{'d':6},
}

def dijkstra(graph,s):
    pqueue = []
    heapq.heappush(pqueue,(0,s))
    visited = set()
    parent = {s:None}
    def init_distance():
        distance = {s:0}
        for v in graph:
            if v != s:
                distance[v] = math.inf
        return distance
    distance = init_distance()
    while(len(pqueue) > 0):
        (dist,v) = heapq.heappop(pqueue)
        if v in visited: continue
        visited.add(v)
        for (vertex,weight) in graph[v].items():
            if vertex not in visited:
                if dist + weight < distance[vertex]:
                    heapq.heappush(pqueue,(dist + weight,vertex))
                    parent[vertex] = v
                    distance[vertex] = dist + weight
    return parent,distance

parent,distance = dijkstra(graph,'a')
def print_shortest_path(parent,distance):
    v = list(parent.keys())[-1]
    shortest_path = distance[v]
    while v:
        print(v,sep='\t')
        v = parent[v]
    print(f"the shortest path length is : {shortest_path}")

print_shortest_path(parent,distance)


