#!/bin/bash


# Load environment modules
module load kvl-remote
module load paraview/4.3.1-openmpi-x86_64

# module load paraview/master-openmpi-x86_64

nodes=4
tasks_per_node=8
total_processes=$[nodes*tasks_per_node]
host_file=/home/srinivm/develop/github/kvl/ParaView/Scripts/launch/gpgpu/hosts

# Define the address of your ParaView client workstation
# madhu-00 desktop
client=109.171.139.1
port=$1
# madhu-01 desktop
#client=109.171.139.2
#client=109.171.138.157
#port=$1

# Run pvserver with OpenMPI
/usr/lib64/openmpi/bin/mpirun -x LD_LIBRARY_PATH -x PATH -x DISPLAY=localhost:0.0 \
    -np $total_processes \
    -hostfile $host_file \
    -npernode $tasks_per_node \
    /var/remote/software/paraview/4.3.1-openmpi-x86_64/bin/pvserver  -display :0.0 \
    --use-offscreen-rendering \
    --reverse-connection \
    --server-port=$port \
    --client-host=$client
