#include "ModelLoader.h"
#include "Core/Logging/Log.h"
#include "Core/Vendor/ufbx.h"
#include <raymath.h>
#include <cstring>
#include <vector>

namespace Core 
{
    Model ModelLoader::LoadModel(std::string_view Path)
    {
        if (IsFileExtension(Path.data(), ".fbx"))
        {
            return LoadFBX(Path);
        }
        else
        {
            // Fallback to Raylib default loader
            return ::LoadModel(Path.data());
        }
    }

    Model ModelLoader::LoadFBX(std::string_view Path)
    {
        ufbx_load_opts opts = {}; 
        opts.target_axes = ufbx_axes_right_handed_y_up; 
        opts.target_unit_meters = 1.0f;

        ufbx_error error;
        ufbx_scene* scene = ufbx_load_file(Path.data(), &opts, &error);

        if (!scene)
        {
            FLog::Error("Failed to load FBX: {}", error.description.data);
            return {};
        }

        std::vector<Mesh> RayMeshes;
        
        for (size_t i = 0; i < scene->nodes.count; i++)
        {
            ufbx_node* node = scene->nodes.data[i];
            if (node->mesh)
            {
                ufbx_mesh* fbx_mesh = node->mesh;
                
                std::vector<Vector3> VPos;
                std::vector<Vector3> VNorm;
                std::vector<Vector2> VUV;
                
                // Using Face Iteration
                for (size_t f = 0; f < fbx_mesh->num_faces; f++)
                {
                    ufbx_face face = fbx_mesh->faces[f];

                    // Fan triangulation
                    for (size_t v = 2; v < face.num_indices; v++)
                    {
                         uint32_t context_indices[3] = 
                        {
                            face.index_begin + 0,
                            face.index_begin + (uint32_t)(v - 1),
                            face.index_begin + (uint32_t)v
                        };
                        
                        for (int k = 0; k < 3; k++)
                        {
                            uint32_t ix = context_indices[k];
                            if (ix < fbx_mesh->vertex_indices.count)
                            {
                                uint32_t v_idx = fbx_mesh->vertex_indices[ix];
                                ufbx_vec3 p = fbx_mesh->vertices[v_idx];
                                VPos.emplace_back
                                (
                                    static_cast<float>(p.x),
                                    static_cast<float>(p.y),
                                    static_cast<float>(p.z)
                                );
                                
                                // Normal
                                if (fbx_mesh->vertex_normal.exists && ix < fbx_mesh->vertex_normal.indices.count)
                                {
                                    uint32_t n_idx = fbx_mesh->vertex_normal.indices[ix];
                                    ufbx_vec3 n = fbx_mesh->vertex_normal.values[n_idx];
                                    VNorm.emplace_back
                                    (
                                        static_cast<float>(n.x),
                                        static_cast<float>(n.y),
                                        static_cast<float>(n.z)
                                    );
                                }
                                
                                // UV
                                if (fbx_mesh->vertex_uv.exists && ix < fbx_mesh->vertex_uv.indices.count)
                                {
                                    uint32_t uv_idx = fbx_mesh->vertex_uv.indices[ix];
                                    ufbx_vec2 uv = fbx_mesh->vertex_uv.values[uv_idx];
                                    VUV.emplace_back
                                    (
                                        static_cast<float>(uv.x), static_cast<float>(uv.y)
                                    );
                                }
                                else
                                {
                                    VUV.emplace_back(0.0f, 0.0f);
                                }
                            }
                        }
                    }
                }
                
                if (VPos.empty()) continue;

                Mesh NewMesh = {};
                NewMesh.vertexCount = static_cast<int>(VPos.size());
                NewMesh.triangleCount = NewMesh.vertexCount / 3;
                
                NewMesh.vertices = static_cast<float*>(MemAlloc(NewMesh.vertexCount * 3 * sizeof(float)));
                memcpy(NewMesh.vertices, VPos.data(), VPos.size() * sizeof(Vector3));
                
                if (!VNorm.empty())
                {
                    NewMesh.normals = static_cast<float*>(MemAlloc(VNorm.size() * 3 * sizeof(float)));
                    memcpy(NewMesh.normals, VNorm.data(), VNorm.size() * sizeof(Vector3));
                }
                
                if (!VUV.empty())
                {
                    NewMesh.texcoords = static_cast<float*>(MemAlloc(VUV.size() * 2 * sizeof(float)));
                    memcpy(NewMesh.texcoords, VUV.data(), VUV.size() * sizeof(Vector2));
                }
                
                UploadMesh(&NewMesh, false);
                RayMeshes.push_back(NewMesh);
            }
        }
        
        ufbx_free_scene(scene);
        
        if (RayMeshes.empty())
        {
             // It's possible to have a hierarchy with no meshes (just bones/empties)
             // We return empty model
             FLog::Debug("FBX loaded but contains no meshes: {}", Path);
             return {};
        }
        
        Model NewModel = {};
        NewModel.meshCount = (int)RayMeshes.size();
        NewModel.meshes = (Mesh*)MemAlloc(NewModel.meshCount * sizeof(Mesh));
        
        // Materials
        NewModel.materialCount = 1;
        NewModel.materials = (Material*)MemAlloc(NewModel.materialCount * sizeof(Material));
        NewModel.materials[0] = LoadMaterialDefault();
        
        for (int i = 0; i < NewModel.meshCount; i++)
        {
            NewModel.meshes[i] = RayMeshes[i];
        }
        
        NewModel.transform = MatrixIdentity();
        
        NewModel.meshMaterial = (int*)MemAlloc(NewModel.meshCount * sizeof(int));
        for (int i = 0; i < NewModel.meshCount; i++)
             NewModel.meshMaterial[i] = 0;
        
        return NewModel;
    }

}
