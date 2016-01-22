//
// Created by Can on 1/22/16.
//

#ifndef HSISOMAP_MANIFOLDCONSTRUCTOR_H
#define HSISOMAP_MANIFOLDCONSTRUCTOR_H

#include <hsisomap/Matrix.h>
#include "../typedefs.h"
#include "../gsl_util/embedding.h"

HSISOMAP_NAMESPACE_BEGIN

std::shared_ptr<gsl::Matrix> ConstructManifold(const gsl::Matrix &landmark_to_all_distances, gsl::Matrix &landmark_distances, gsl::Embedding &landmark_cmds_embedding, Index reduced_dimensions);

HSISOMAP_NAMESPACE_END

#endif //HSISOMAP_MANIFOLDCONSTRUCTOR_H
