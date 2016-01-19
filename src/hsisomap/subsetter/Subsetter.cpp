//
// Created by Can on 1/18/16.
//

#include <hsisomap/subsetter/Subsetter.h>
#include <hsisomap/subsetter/SubsetterEmbedding.h>

HSISOMAP_NAMESPACE_BEGIN

std::shared_ptr<hsisomap::Subsetter> SubsetterWithImplementation(SubsetterImplementation subsetter_implementation,
                                                                 std::shared_ptr<gsl::Matrix> data,
                                                                 PropertyList property_list,
                                                                 std::shared_ptr<gsl::Matrix> optional_embedding) {

  switch (subsetter_implementation) {
    case SUBSETTER_IMPLEMENTATION_RANDOMSKEL:
      throw std::invalid_argument("Subsetter Implementation RandomSkel not supported yet.");
    case SUBSETTER_IMPLEMENTATION_EMBEDDING:
      return std::dynamic_pointer_cast<Subsetter>(std::make_shared<SubsetterEmbedding>(data, property_list, optional_embedding));
  }
return
std::shared_ptr<Subsetter>();
}

HSISOMAP_NAMESPACE_END

