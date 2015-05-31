#!/bin/bash

# Load environment modules
module load paraview/4.3.1-openmpi-dev

nodes=8
tasks_per_node=8
total_processes=$[nodes*tasks_per_node]
host_file=/home/srinivm/develop/github/kvl/ParaView/Scripts/launch/z5/hosts

port=11111

# Run pvserver with OpenMPI
/usr/lib64/openmpi/bin/mpirun -x LD_LIBRARY_PATH -x PATH  \
    -np $total_processes \
    -hostfile $host_file \
    -npernode $tasks_per_node \
    /home/srinivm/software/paraview/4.3.1/bin/pvserver \
    --data-server-port=$port
