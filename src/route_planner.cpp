#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    this->start_node = &m_Model.FindClosestNode(start_x, start_y);
    this->end_node = &m_Model.FindClosestNode(end_x, end_y);
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    //TODO check end_node is valid?
    return node->distance(*this->end_node);

}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    if(current_node == nullptr)
        return;
    current_node->FindNeighbors();
    for_each(begin(current_node->neighbors), end(current_node->neighbors), [&](RouteModel::Node* elem){
        if(!elem->visited){
        elem->parent = current_node;
        elem->g_value = current_node->g_value + current_node->distance(*elem);
        elem->h_value = CalculateHValue(elem);
        elem->visited = true;
        this->open_list.emplace_back(elem);
        }
    });

}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {
  //TODO why sort over and over agian.
    std::sort(begin(this->open_list), end(this->open_list), [&](RouteModel::Node* elem1, RouteModel::Node* elem2){
        return (elem1->g_value+elem1->h_value) > (elem2->g_value+elem2->h_value);
    });
    RouteModel::Node* last = this->open_list.back();
    this->open_list.pop_back();
    return last;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    if(!current_node)
        return path_found;
    // TODO: Implement your solution here.
    while(current_node != start_node){
        if(!current_node->parent)
            return std::vector<RouteModel::Node>();
        distance += current_node->distance(*current_node->parent);
        path_found.emplace_back(*current_node);
        current_node = current_node->parent;
    }
    path_found.emplace_back(*current_node);
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return {rbegin(path_found), rend(path_found)};

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = start_node;
    open_list.push_back(start_node);
    start_node->visited = true;
    // TODO: Implement your solution here.
    while(open_list.size() ){
     //   if(current_node->parent == nullptr){
       //     std::cout << "\n No path found";
         //   return;
        //}
        current_node->visited = true;
        if(current_node->distance(*end_node) == 0){
            m_Model.path = this->ConstructFinalPath(end_node);
            return;
        }
        //populate open list with neighbors
        this->AddNeighbors(current_node);
        current_node = this->NextNode();
        
        // check for no path cases
    }
    
    

}
