Voxelizer
=========

A fast GPU based solid voxelization library written in C.


Notes:
- Only supports Windows at the moment. I plan to add Linux and Mac support eventually.
- Requires a GPU supporting Opengl 4.2 with the GL_ARB_shader_storage_buffer_object extension. Future versions should include algorithms with less demanding GPU requirements.
- The only output data structure supported right now is a linked list of the starting and ending voxels for each row and column. Example.c shows how to voxelize a mesh and read the linked list.
- Input triangle meshes must be watertight, and each vertex component must be in the interval [-1, 1]. Though, the algorithm does not clip the depth component in the voxel grid, so the voxelizer can currently output an infinite number of voxels along the z axis. This is because I have not had a chance to implement this, but some might actually consider this a feature.
