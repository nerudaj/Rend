#pragma once

#include <SFML/Config.hpp>
#include <bitset>
#include <cstddef>
#include <dgm/classes/Objects.hpp>

using EntityIndexType = std::size_t;
using MarkerIndexType = std::size_t;
using PlayerStateIndexType = std::size_t;
using WeaponIndexType = std::size_t;
using AcquitedWeaponsArray = std::bitset<8>;
using AmmoCounterType = int;
using LightType = sf::Uint8;
using LightmapType = dgm::GenericMesh<LightType>;
using MeshItrType = decltype(std::remove_cvref_t<std::invoke_result_t<
                                 decltype(&dgm::Mesh::getDataSize),
                                 dgm::Mesh>>::x);