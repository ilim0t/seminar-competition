#include "../algorithm.h"
#include "../cpu_time.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>

void two_opt_algorithm(const Param* const param,
                       const TSPdata* const tspdata,
                       Vdata* const vdata) {
  for (int i = 0; i < tspdata->n; i++) {
    vdata->bestsol[i] = -1;
  }
  nearest_neighbor(tspdata->n, tspdata->min_node_num, param->timelim * 0.1,
                   tspdata->x, tspdata->y, vdata->bestsol);
  insertion(tspdata->n, tspdata->min_node_num, param->timelim * 0.1, tspdata->x,
            tspdata->y, vdata->bestsol);

  two_opt(tspdata->n, tspdata->min_node_num,
          param->timelim - cpu_time() - vdata->starttime, tspdata->x,
          tspdata->y, vdata->bestsol);
}

void two_opt(int n_nodes,
             int n_min_nodes,
             double timelim,
             double x_coords[n_nodes],
             double y_coords[n_nodes],
             int best_tour[n_nodes]) {
  double starttime = cpu_time();

  while (cpu_time() - starttime < timelim) {
    const int a_tour_idx = rand() % n_min_nodes;
    const int c_tour_idx = (a_tour_idx + 1) % n_min_nodes;

    const int b_tour_idx =
        (a_tour_idx + 1 + rand() % (n_min_nodes - 1)) % n_min_nodes;
    const int d_tour_idx = (b_tour_idx + 1) % n_min_nodes;

    const int a_node = best_tour[a_tour_idx];
    const int b_node = best_tour[b_tour_idx];
    const int c_node = best_tour[c_tour_idx];
    const int d_node = best_tour[d_tour_idx];

    const int reduces_cost = my_dist(x_coords, y_coords, a_node, c_node) +
                             my_dist(x_coords, y_coords, b_node, d_node) -
                             my_dist(x_coords, y_coords, a_node, b_node) -
                             my_dist(x_coords, y_coords, c_node, d_node);
    if (reduces_cost <= 0) {
      continue;
    }

    int new_tour[n_min_nodes];
    int new_tour_idx = 0;
#if DEBUG
    for (int i = 0; i < n_min_nodes; i++) {
      new_tour[i] = -1;
    }
#endif

    for (int tour_idx = b_tour_idx; tour_idx != c_tour_idx;
         tour_idx = tour_idx == 0 ? n_min_nodes - 1 : (tour_idx - 1)) {
      new_tour[new_tour_idx] = best_tour[tour_idx];
      new_tour_idx++;
    }
    new_tour[new_tour_idx] = c_node;
    new_tour_idx++;

    for (int tour_idx = d_tour_idx; tour_idx != a_tour_idx;
         tour_idx = (tour_idx + 1) % n_min_nodes) {
      new_tour[new_tour_idx] = best_tour[tour_idx];
      new_tour_idx++;
    }
    new_tour[new_tour_idx] = a_node;
    // new_tour_idx++;

    for (int tour_idx = 0; tour_idx < n_min_nodes; tour_idx++) {
      best_tour[tour_idx] = new_tour[tour_idx];
    }
#if DEBUG
    printf("\n[UPDATE] (reduces_cost=%d)\n", reduces_cost);
    print_tour(n_nodes, x_coords, y_coords, best_tour);
    printf("\n");
    printf(" ");
#endif
  }
}