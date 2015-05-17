#!/bin/bash


# Load environment modules
module load kvl-remote
module load paraview/4.3.1

nodes=10
tasks_per_node=8
total_processes=$[nodes*tasks_per_node]

# Define the address of your ParaView client workstation
# madhu-00 desktop
#client=109.171.139.1
#port=$1
# madhu-01 desktop
#client=109.171.139.2
client=109.171.138.145
port=$1


# Run pvserver with mpich2
mpirun -genvlist LD_LIBRARY_PATH,PATH,PYTHONPATH \
    -genv MV2_ENABLE_AFFINITY 0 -genv IPATH_NO_CPUAFFINITY 1 \
    -np $total_processes \
    -hosts z5-0-0,z5-0-1,z5-0-2,z5-1-0,z5-1-1,z5-1-2,z5-2-0,z5-2-1,z5-2-2,z5-3-0 \
    -ppn $tasks_per_node \
    /var/remote/software/paraview/4.3.1/bin/pvserver -display :0.0 \
    --use-offscreen-rendering \
    --reverse-connection \
    --server-port=$port \
    --client-host=$client
