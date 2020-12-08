#include "../algorithm.h"
#include "../cpu_time.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

void three_opt_algorithm(const Param* const param,
                         const TSPdata* const tspdata,
                         Vdata* const vdata) {
  for (int i = 0; i < tspdata->n; i++) {
    vdata->bestsol[i] = -1;
  }

  nearest_neighbor(tspdata->n, tspdata->min_node_num,
                   (param->timelim - cpu_time() + vdata->starttime) * 0.1,
                   tspdata->x, tspdata->y, vdata->bestsol);

  const double iter_tim_lim =
      (param->timelim - cpu_time() + vdata->starttime) / 20;

  while (cpu_time() - vdata->starttime < param->timelim) {
    if (tspdata->n > tspdata->min_node_num) {
      replace(tspdata->n, tspdata->min_node_num,
              fmin(iter_tim_lim / 4,
                   param->timelim - cpu_time() + vdata->starttime),
              tspdata->x, tspdata->y, vdata->bestsol);
    }
    two_opt(
        tspdata->n, tspdata->min_node_num,
        fmin(iter_tim_lim / 2, param->timelim - cpu_time() + vdata->starttime),
        tspdata->x, tspdata->y, vdata->bestsol);
    three_opt(
        tspdata->n, tspdata->min_node_num,
        fmin(iter_tim_lim / 4, param->timelim - cpu_time() + vdata->starttime),
        tspdata->x, tspdata->y, vdata->bestsol);
  }
}

void three_opt(int n_nodes,
               int n_min_nodes,
               double timelim,
               double x_coords[n_nodes],
               double y_coords[n_nodes],
               int best_tour[n_nodes]) {
  double starttime = cpu_time();

  while (cpu_time() - starttime < timelim) {
    int a_tour_idx, b_tour_idx, c_tour_idx;
    while (true) {
      a_tour_idx = rand() % n_min_nodes;
      b_tour_idx = rand() % n_min_nodes;
      c_tour_idx = rand() % n_min_nodes;

      if (a_tour_idx == b_tour_idx || b_tour_idx == c_tour_idx ||
          c_tour_idx == a_tour_idx) {
        continue;
      } else if ((a_tour_idx < b_tour_idx) + (b_tour_idx < c_tour_idx) +
                     (c_tour_idx < a_tour_idx) ==
                 1) {
        continue;
      }
      break;
    }
    const int d_tour_idx = (a_tour_idx + 1) % n_min_nodes;
    const int e_tour_idx = (b_tour_idx + 1) % n_min_nodes;
    const int f_tour_idx = (c_tour_idx + 1) % n_min_nodes;

    const int a_node = best_tour[a_tour_idx];
    const int d_node = best_tour[d_tour_idx];

    const int b_node = best_tour[b_tour_idx];
    const int e_node = best_tour[e_tour_idx];

    const int c_node = best_tour[c_tour_idx];
    const int f_node = best_tour[f_tour_idx];

    const int oririnal_cost = my_dist(x_coords, y_coords, a_node, d_node) +
                              my_dist(x_coords, y_coords, b_node, e_node) +
                              my_dist(x_coords, y_coords, c_node, f_node);

    int new_costs[4];
    new_costs[0] = my_dist(x_coords, y_coords, b_node, c_node) +
                   my_dist(x_coords, y_coords, e_node, a_node) +
                   my_dist(x_coords, y_coords, f_node, d_node);

    new_costs[1] = my_dist(x_coords, y_coords, b_node, a_node) +
                   my_dist(x_coords, y_coords, f_node, e_node) +
                   my_dist(x_coords, y_coords, c_node, d_node);

    new_costs[2] = my_dist(x_coords, y_coords, b_node, f_node) +
                   my_dist(x_coords, y_coords, a_node, e_node) +
                   my_dist(x_coords, y_coords, c_node, d_node);

    new_costs[3] = my_dist(x_coords, y_coords, b_node, f_node) +
                   my_dist(x_coords, y_coords, a_node, c_node) +
                   my_dist(x_coords, y_coords, e_node, d_node);

    int min_cost_idx;
    int min_new_cost = INT_MAX;
    for (int i = 0; i < 4; i++) {
      if (new_costs[i] < min_new_cost) {
        min_new_cost = new_costs[i];
        min_cost_idx = i;
      }
    }

    const int reduced_cost = oririnal_cost - min_new_cost;
    if (reduced_cost <= 0) {
      continue;
    }

    int new_tour[n_min_nodes];
    int new_tour_idx = 0;
#if DEBUG
    for (int i = 0; i < n_min_nodes; i++) {
      new_tour[i] = -1;
    }
#endif

    for (int tour_idx = d_tour_idx; tour_idx != b_tour_idx;
         tour_idx = (tour_idx + 1) % n_min_nodes) {
      new_tour[new_tour_idx] = best_tour[tour_idx];
      new_tour_idx++;
    }

    new_tour[new_tour_idx] = b_node;
    new_tour_idx++;

    if (min_cost_idx == 0) {
      for (int tour_idx = c_tour_idx; tour_idx != e_tour_idx;
           tour_idx = tour_idx == 0 ? n_min_nodes - 1 : (tour_idx - 1)) {
        new_tour[new_tour_idx] = best_tour[tour_idx];
        new_tour_idx++;
      }

      new_tour[new_tour_idx] = e_node;
      new_tour_idx++;

      for (int tour_idx = a_tour_idx; tour_idx != f_tour_idx;
           tour_idx = tour_idx == 0 ? n_min_nodes - 1 : (tour_idx - 1)) {
        new_tour[new_tour_idx] = best_tour[tour_idx];
        new_tour_idx++;
      }

      new_tour[new_tour_idx] = f_node;
      // new_tour_idx++;
    } else if (min_cost_idx == 1) {
      for (int tour_idx = a_tour_idx; tour_idx != f_tour_idx;
           tour_idx = tour_idx == 0 ? n_min_nodes - 1 : (tour_idx - 1)) {
        new_tour[new_tour_idx] = best_tour[tour_idx];
        new_tour_idx++;
      }
      new_tour[new_tour_idx] = f_node;
      new_tour_idx++;

      for (int tour_idx = e_tour_idx; tour_idx != c_tour_idx;
           tour_idx = (tour_idx + 1) % n_min_nodes) {
        new_tour[new_tour_idx] = best_tour[tour_idx];
        new_tour_idx++;
      }

      new_tour[new_tour_idx] = c_node;
      // new_tour_idx++;
    } else {
      for (int tour_idx = f_tour_idx; tour_idx != a_tour_idx;
           tour_idx = (tour_idx + 1) % n_min_nodes) {
        new_tour[new_tour_idx] = best_tour[tour_idx];
        new_tour_idx++;
      }

      new_tour[new_tour_idx] = a_node;
      new_tour_idx++;
      if (min_cost_idx == 2) {
        for (int tour_idx = e_tour_idx; tour_idx != c_tour_idx;
             tour_idx = (tour_idx + 1) % n_min_nodes) {
          new_tour[new_tour_idx] = best_tour[tour_idx];
          new_tour_idx++;
        }

        new_tour[new_tour_idx] = c_node;
        // new_tour_idx++;
      } else {
        for (int tour_idx = c_tour_idx; tour_idx != e_tour_idx;
             tour_idx = tour_idx == 0 ? n_min_nodes - 1 : (tour_idx - 1)) {
          new_tour[new_tour_idx] = best_tour[tour_idx];
          new_tour_idx++;
        }
        new_tour[new_tour_idx] = e_node;
        // new_tour_idx++;
      }
    }

    for (int tour_idx = 0; tour_idx < n_min_nodes; tour_idx++) {
      best_tour[tour_idx] = new_tour[tour_idx];
    }

#if DEBUG
    printf("\n[UPDATE] three_opt (reduced_cost=%d)\n", reduced_cost);
    print_tour(n_nodes, n_min_nodes, x_coords, y_coords, best_tour);
    printf("\n");
#endif
  }
}