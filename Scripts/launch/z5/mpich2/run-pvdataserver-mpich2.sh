#!/bin/bash


# Load environment modules
module load kvl-remote
module load paraview/4.3.1

nodes=10
tasks_per_node=8
total_processes=$[nodes*tasks_per_node]
port=11111

# Run pvrenderserver with mpich2
mpirun -genv MV2_ENABLE_AFFINITY 0 -genv IPATH_NO_CPUAFFINITY 1 \
    -np $total_processes \
    -hosts z5-0-0,z5-0-1,z5-0-2,z5-1-0,z5-1-1,z5-1-2,z5-2-0,z5-2-1,z5-2-2,z5-3-0 \
    -ppn $tasks_per_node \
    /var/remote/software/paraview/4.3.1/bin/pvdataserver \
    --data-server-port=$port
