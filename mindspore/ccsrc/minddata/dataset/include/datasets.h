/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_CCSRC_MINDDATA_DATASET_INCLUDE_DATASETS_H_
#define MINDSPORE_CCSRC_MINDDATA_DATASET_INCLUDE_DATASETS_H_

#include <unistd.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "minddata/dataset/core/constants.h"
#include "minddata/dataset/engine/data_schema.h"
#include "minddata/dataset/include/iterator.h"
#include "minddata/dataset/include/samplers.h"
#include "minddata/dataset/include/tensor.h"
#include "minddata/dataset/include/type_id.h"
#include "minddata/dataset/kernels/c_func_op.h"
#include "minddata/dataset/kernels/tensor_op.h"
#include "minddata/dataset/util/path.h"
#ifndef ENABLE_ANDROID
#include "minddata/dataset/text/vocab.h"
#endif

namespace mindspore {
namespace dataset {

// Forward declare
class DatasetOp;
class DataSchema;
class Tensor;
class TensorShape;
class TreeAdapter;
#ifndef ENABLE_ANDROID
class Vocab;
#endif

namespace api {

class TensorOperation;
class SchemaObj;
class SamplerObj;
// Datasets classes (in alphabetical order)
class AlbumNode;
class CelebANode;
class Cifar10Node;
class Cifar100Node;
class CLUENode;
class CocoNode;
class CSVNode;
class CsvBase;
class ImageFolderNode;
#ifndef ENABLE_ANDROID
class ManifestNode;
class MindDataNode;
#endif
class MnistNode;
class RandomNode;
class TextFileNode;
#ifndef ENABLE_ANDROID
class TFRecordNode;
class VOCNode;
#endif
// Dataset Op classes (in alphabetical order)
class BatchNode;
#ifndef ENABLE_ANDROID
class BucketBatchByLengthNode;
class BuildVocabNode;
#endif
class ConcatNode;
class MapNode;
class ProjectNode;
class RenameNode;
class RepeatNode;
class ShuffleNode;
class SkipNode;
class TakeNode;
class ZipNode;

/// \brief Function to create a SchemaObj
/// \param[in] schema_file Path of schema file
/// \return Shared pointer to the current schema
std::shared_ptr<SchemaObj> Schema(const std::string &schema_file = "");

/// \brief Function to create an AlbumNode
/// \notes The generated dataset is specified through setting a schema
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] data_schema Path to dataset schema file
/// \param[in] column_names Column names used to specify columns to load, if empty, will read all columns.
///     (default = {})
/// \param[in] decode the option to decode the images in dataset (default = false)
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current Dataset
std::shared_ptr<AlbumNode> Album(const std::string &dataset_dir, const std::string &data_schema,
                                 const std::vector<std::string> &column_names = {}, bool decode = false,
                                 const std::shared_ptr<SamplerObj> &sampler = RandomSampler());

/// \brief Function to create a CelebANode
/// \notes The generated dataset has two columns ['image', 'attr'].
///      The type of the image tensor is uint8. The attr tensor is uint32 and one hot type.
/// \param[in] dataset_dir Path to the root directory that contains the dataset.
/// \param[in] usage One of "all", "train", "valid" or "test" (default = "all").
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \param[in] decode Decode the images after reading (default=false).
/// \param[in] extensions Set of file extensions to be included in the dataset (default={}).
/// \return Shared pointer to the current Dataset
std::shared_ptr<CelebANode> CelebA(const std::string &dataset_dir, const std::string &usage = "all",
                                   const std::shared_ptr<SamplerObj> &sampler = RandomSampler(), bool decode = false,
                                   const std::set<std::string> &extensions = {});

/// \brief Function to create a Cifar10 Dataset
/// \notes The generated dataset has two columns ["image", "label"]
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] usage of CIFAR10, can be "train", "test" or "all" (default = "all").
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current Dataset
std::shared_ptr<Cifar10Node> Cifar10(const std::string &dataset_dir, const std::string &usage = "all",
                                     const std::shared_ptr<SamplerObj> &sampler = RandomSampler());

/// \brief Function to create a Cifar100 Dataset
/// \notes The generated dataset has three columns ["image", "coarse_label", "fine_label"]
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] usage of CIFAR100, can be "train", "test" or "all" (default = "all").
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current Dataset
std::shared_ptr<Cifar100Node> Cifar100(const std::string &dataset_dir, const std::string &usage = "all",
                                       const std::shared_ptr<SamplerObj> &sampler = RandomSampler());

/// \brief Function to create a CLUENode
/// \notes The generated dataset has a variable number of columns depending on the task and usage
/// \param[in] dataset_files List of files to be read to search for a pattern of files. The list
///     will be sorted in a lexicographical order.
/// \param[in] task The kind of task, one of "AFQMC", "TNEWS", "IFLYTEK", "CMNLI", "WSC" and "CSL" (default="AFQMC").
/// \param[in] usage Be used to "train", "test" or "eval" data (default="train").
/// \param[in] num_samples The number of samples to be included in the dataset.
///     (Default = 0 means all samples.)
/// \param[in] shuffle The mode for shuffling data every epoch. (Default=ShuffleMode.kGlobal)
///     Can be any of:
///     ShuffleMode::kFalse - No shuffling is performed.
///     ShuffleMode::kFiles - Shuffle files only.
///     ShuffleMode::kGlobal - Shuffle both the files and samples.
/// \param[in] num_shards Number of shards that the dataset should be divided into. (Default = 1)
/// \param[in] shard_id The shard ID within num_shards. This argument should be
///     specified only when num_shards is also specified. (Default = 0)
/// \return Shared pointer to the current CLUENode
std::shared_ptr<CLUENode> CLUE(const std::vector<std::string> &dataset_files, const std::string &task = "AFQMC",
                               const std::string &usage = "train", int64_t num_samples = 0,
                               ShuffleMode shuffle = ShuffleMode::kGlobal, int32_t num_shards = 1,
                               int32_t shard_id = 0);

/// \brief Function to create a CocoNode
/// \notes The generated dataset has multi-columns :
///     - task='Detection', column: [['image', dtype=uint8], ['bbox', dtype=float32], ['category_id', dtype=uint32],
///                                  ['iscrowd', dtype=uint32]].
///     - task='Stuff', column: [['image', dtype=uint8], ['segmentation',dtype=float32], ['iscrowd', dtype=uint32]].
///     - task='Keypoint', column: [['image', dtype=uint8], ['keypoints', dtype=float32],
///                                 ['num_keypoints', dtype=uint32]].
///     - task='Panoptic', column: [['image', dtype=uint8], ['bbox', dtype=float32], ['category_id', dtype=uint32],
///                                 ['iscrowd', dtype=uint32], ['area', dtype=uitn32]].
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] annotation_file Path to the annotation json
/// \param[in] task Set the task type of reading coco data, now support 'Detection'/'Stuff'/'Panoptic'/'Keypoint'
/// \param[in] decode Decode the images after reading
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current Dataset
std::shared_ptr<CocoNode> Coco(const std::string &dataset_dir, const std::string &annotation_file,
                               const std::string &task = "Detection", const bool &decode = false,
                               const std::shared_ptr<SamplerObj> &sampler = RandomSampler());

/// \brief Function to create a CSVNode
/// \notes The generated dataset has a variable number of columns
/// \param[in] dataset_files List of files to be read to search for a pattern of files. The list
///    will be sorted in a lexicographical order.
/// \param[in] field_delim A char that indicates the delimiter to separate fields (default=',').
/// \param[in] column_defaults List of default values for the CSV field (default={}). Each item in the list is
///    either a valid type (float, int, or string). If this is not provided, treats all columns as string type.
/// \param[in] column_names List of column names of the dataset (default={}). If this is not provided, infers the
///    column_names from the first row of CSV file.
/// \param[in] num_samples The number of samples to be included in the dataset.
///    (Default = 0 means all samples.)
/// \param[in] shuffle The mode for shuffling data every epoch. (Default=ShuffleMode::kGlobal)
///    Can be any of:
///    ShuffleMode::kFalse - No shuffling is performed.
///    ShuffleMode::kFiles - Shuffle files only.
///    ShuffleMode::kGlobal - Shuffle both the files and samples.
/// \param[in] num_shards Number of shards that the dataset should be divided into. (Default = 1)
/// \param[in] shard_id The shard ID within num_shards. This argument should be
///    specified only when num_shards is also specified. (Default = 0)
/// \return Shared pointer to the current Dataset
std::shared_ptr<CSVNode> CSV(const std::vector<std::string> &dataset_files, char field_delim = ',',
                             const std::vector<std::shared_ptr<CsvBase>> &column_defaults = {},
                             const std::vector<std::string> &column_names = {}, int64_t num_samples = 0,
                             ShuffleMode shuffle = ShuffleMode::kGlobal, int32_t num_shards = 1, int32_t shard_id = 0);

/// \brief Function to create an ImageFolderNode
/// \notes A source dataset that reads images from a tree of directories
///     All images within one folder have the same label
///     The generated dataset has two columns ["image", "label"]
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] decode A flag to decode in ImageFolder
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \param[in] extensions File extensions to be read
/// \param[in] class_indexing a class name to label map
/// \return Shared pointer to the current ImageFolderNode
std::shared_ptr<ImageFolderNode> ImageFolder(const std::string &dataset_dir, bool decode = false,
                                             const std::shared_ptr<SamplerObj> &sampler = RandomSampler(),
                                             const std::set<std::string> &extensions = {},
                                             const std::map<std::string, int32_t> &class_indexing = {});

#ifndef ENABLE_ANDROID
/// \brief Function to create a ManifestNode
/// \notes The generated dataset has two columns ["image", "label"]
/// \param[in] dataset_file The dataset file to be read
/// \param[in] usage Need "train", "eval" or "inference" data (default="train")
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \param[in] class_indexing A str-to-int mapping from label name to index (default={}, the folder
///     names will be sorted alphabetically and each class will be given a unique index starting from 0).
/// \param[in] decode Decode the images after reading (default=false).
/// \return Shared pointer to the current ManifestNode
std::shared_ptr<ManifestNode> Manifest(const std::string &dataset_file, const std::string &usage = "train",
                                       const std::shared_ptr<SamplerObj> &sampler = RandomSampler(),
                                       const std::map<std::string, int32_t> &class_indexing = {}, bool decode = false);
#endif

#ifndef ENABLE_ANDROID
/// \brief Function to create a MindDataNode
/// \param[in] dataset_file File name of one component of a mindrecord source. Other files with identical source
///     in the same path will be found and loaded automatically.
/// \param[in] columns_list List of columns to be read (default={})
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler()),
///     supported sampler list: SubsetRandomSampler, PkSampler, RandomSampler, SequentialSampler, DistributedSampler.
/// \param[in] padded_sample Samples will be appended to dataset, where keys are the same as column_list.
/// \param[in] num_padded Number of padding samples. Dataset size plus num_padded should be divisible by num_shards.
/// \return Shared pointer to the current MindDataNode
std::shared_ptr<MindDataNode> MindData(const std::string &dataset_file,
                                       const std::vector<std::string> &columns_list = {},
                                       const std::shared_ptr<SamplerObj> &sampler = RandomSampler(),
                                       nlohmann::json padded_sample = nullptr, int64_t num_padded = 0);

/// \brief Function to create a MindDataNode
/// \param[in] dataset_files List of dataset files to be read directly.
/// \param[in] columns_list List of columns to be read (default={})
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler()),
///     supported sampler list: SubsetRandomSampler, PkSampler, RandomSampler, SequentialSampler, DistributedSampler.
/// \param[in] padded_sample Samples will be appended to dataset, where keys are the same as column_list.
/// \param[in] num_padded Number of padding samples. Dataset size plus num_padded should be divisible by num_shards.
/// \return Shared pointer to the current MindDataNode
std::shared_ptr<MindDataNode> MindData(const std::vector<std::string> &dataset_files,
                                       const std::vector<std::string> &columns_list = {},
                                       const std::shared_ptr<SamplerObj> &sampler = RandomSampler(),
                                       nlohmann::json padded_sample = nullptr, int64_t num_padded = 0);
#endif

/// \brief Function to create a MnistNode
/// \notes The generated dataset has two columns ["image", "label"]
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] usage of MNIST, can be "train", "test" or "all" (default = "all").
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current MnistNode
std::shared_ptr<MnistNode> Mnist(const std::string &dataset_dir, const std::string &usage = "all",
                                 const std::shared_ptr<SamplerObj> &sampler = RandomSampler());

/// \brief Function to create a ConcatNode
/// \notes Reload "+" operator to concat two datasets
/// \param[in] datasets1 Shared pointer to the first dataset to be concatenated
/// \param[in] datasets2 Shared pointer to the second dataset to be concatenated
/// \return Shared pointer to the current ConcatNode
std::shared_ptr<ConcatNode> operator+(const std::shared_ptr<Dataset> &datasets1,
                                      const std::shared_ptr<Dataset> &datasets2);

/// \brief Function to create a RandomNode
/// \param[in] total_rows Number of rows for the dataset to generate (default=0, number of rows is random)
/// \param[in] schema SchemaObj to set column type, data type and data shape
/// \param[in] columns_list List of columns to be read (default={}, read all columns)
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current Dataset
template <typename T = std::shared_ptr<SchemaObj>>
std::shared_ptr<RandomNode> RandomData(const int32_t &total_rows = 0, const T &schema = nullptr,
                                       const std::vector<std::string> &columns_list = {},
                                       const std::shared_ptr<SamplerObj> &sampler = RandomSampler()) {
  if (total_rows < 0) {
    MS_LOG(ERROR) << "RandomNode: total_rows must be greater than or equal 0, now get " << total_rows;
    return nullptr;
  }
  if (sampler == nullptr) {
    MS_LOG(ERROR) << "RandomNode: Sampler is not constructed correctly, sampler: nullptr";
    return nullptr;
  }
  if (!columns_list.empty()) {
    for (uint32_t i = 0; i < columns_list.size(); ++i) {
      if (columns_list[i].empty()) {
        MS_LOG(ERROR) << "RandomNode:columns_list"
                      << "[" << i << "] should not be empty";
        return nullptr;
      }
    }
    std::set<std::string> columns_set(columns_list.begin(), columns_list.end());
    if (columns_set.size() != columns_list.size()) {
      MS_LOG(ERROR) << "RandomNode:columns_list: Every column name should not be same with others";
      return nullptr;
    }
  }
  std::shared_ptr<RandomNode> ds;
  if constexpr (std::is_same<T, std::nullptr_t>::value || std::is_same<T, std::shared_ptr<SchemaObj>>::value) {
    std::shared_ptr<SchemaObj> schema_obj = schema;
    ds = std::make_shared<RandomNode>(total_rows, std::move(schema_obj), std::move(columns_list), std::move(sampler));
  } else {
    ds = std::make_shared<RandomNode>(total_rows, std::move(schema), std::move(columns_list), std::move(sampler));
  }
  return ds;
}

/// \brief Function to create a TextFileNode
/// \notes The generated dataset has one column ['text']
/// \param[in] dataset_files List of files to be read to search for a pattern of files. The list
///     will be sorted in a lexicographical order.
/// \param[in] num_samples The number of samples to be included in the dataset.
///     (Default = 0 means all samples.)
/// \param[in] shuffle The mode for shuffling data every epoch. (Default=ShuffleMode.kGlobal)
///     Can be any of:
///     ShuffleMode.kFalse - No shuffling is performed.
///     ShuffleMode.kFiles - Shuffle files only.
///     ShuffleMode.kGlobal - Shuffle both the files and samples.
/// \param[in] num_shards Number of shards that the dataset should be divided into. (Default = 1)
/// \param[in] shard_id The shard ID within num_shards. This argument should be
///     specified only when num_shards is also specified. (Default = 0)
/// \return Shared pointer to the current TextFileNode
std::shared_ptr<TextFileNode> TextFile(const std::vector<std::string> &dataset_files, int64_t num_samples = 0,
                                       ShuffleMode shuffle = ShuffleMode::kGlobal, int32_t num_shards = 1,
                                       int32_t shard_id = 0);

#ifndef ENABLE_ANDROID
/// \brief Function to create a TFRecordNode
/// \param[in] dataset_files List of files to be read to search for a pattern of files. The list
///     will be sorted in a lexicographical order.
/// \param[in] schema SchemaObj or string to schema path. (Default = nullptr, which means that the
///     meta data from the TFData file is considered the schema.)
/// \param[in] columns_list List of columns to be read. (Default = {}, read all columns)
/// \param[in] num_samples The number of samples to be included in the dataset.
///     (Default = 0 means all samples.)
///     If num_samples is 0 and numRows(parsed from schema) does not exist, read the full dataset;
///     If num_samples is 0 and numRows(parsed from schema) is greater than 0, read numRows rows;
///     If both num_samples and numRows(parsed from schema) are greater than 0, read num_samples rows.
/// \param[in] shuffle The mode for shuffling data every epoch. (Default = ShuffleMode::kGlobal)
///     Can be any of:
///     ShuffleMode::kFalse - No shuffling is performed.
///     ShuffleMode::kFiles - Shuffle files only.
///     ShuffleMode::kGlobal - Shuffle both the files and samples.
/// \param[in] num_shards Number of shards that the dataset should be divided into. (Default = 1)
/// \param[in] shard_id The shard ID within num_shards. This argument should be specified only
///     when num_shards is also specified. (Default = 0)
/// \param[in] shard_equal_rows Get equal rows for all shards. (Default = False, number of rows of
///     each shard may be not equal)
/// \return Shared pointer to the current TFRecordNode
template <typename T = std::shared_ptr<SchemaObj>>
std::shared_ptr<TFRecordNode> TFRecord(const std::vector<std::string> &dataset_files, const T &schema = nullptr,
                                       const std::vector<std::string> &columns_list = {}, int64_t num_samples = 0,
                                       ShuffleMode shuffle = ShuffleMode::kGlobal, int32_t num_shards = 1,
                                       int32_t shard_id = 0, bool shard_equal_rows = false) {
  if (dataset_files.empty()) {
    MS_LOG(ERROR) << "TFRecordNode: dataset_files is not specified.";
    return nullptr;
  }

  for (auto f : dataset_files) {
    Path dataset_file(f);
    if (!dataset_file.Exists()) {
      MS_LOG(ERROR) << "TFRecordNode: dataset file: [" << f << "] is invalid or does not exist.";
      return nullptr;
    }
  }

  if (num_samples < 0) {
    MS_LOG(ERROR) << "TFRecordNode: Invalid number of samples: " << num_samples;
    return nullptr;
  }

  if (num_shards <= 0) {
    MS_LOG(ERROR) << "TFRecordNode: Invalid num_shards: " << num_shards;
    return nullptr;
  }

  if (shard_id < 0 || shard_id >= num_shards) {
    MS_LOG(ERROR) << "TFRecordNode: Invalid input, shard_id: " << shard_id << ", num_shards: " << num_shards;
    return nullptr;
  }
  std::shared_ptr<TFRecordNode> ds = nullptr;
  if constexpr (std::is_same<T, std::nullptr_t>::value || std::is_same<T, std::shared_ptr<SchemaObj>>::value) {
    std::shared_ptr<SchemaObj> schema_obj = schema;
    ds = std::make_shared<TFRecordNode>(dataset_files, schema_obj, columns_list, num_samples, shuffle, num_shards,
                                        shard_id, shard_equal_rows);
  } else {
    std::string schema_path = schema;
    if (!schema_path.empty()) {
      Path schema_file(schema_path);
      if (!schema_file.Exists()) {
        MS_LOG(ERROR) << "TFRecordNode: schema path [" << schema_path << "] is invalid or does not exist.";
        return nullptr;
      }
    }
    ds = std::make_shared<TFRecordNode>(dataset_files, schema_path, columns_list, num_samples, shuffle, num_shards,
                                        shard_id, shard_equal_rows);
  }
  return ds;
}

/// \brief Function to create a VOCNode
/// \notes The generated dataset has multi-columns :
///     - task='Detection', column: [['image', dtype=uint8], ['bbox', dtype=float32], ['label', dtype=uint32],
///                                  ['difficult', dtype=uint32], ['truncate', dtype=uint32]].
///     - task='Segmentation', column: [['image', dtype=uint8], ['target',dtype=uint8]].
/// \param[in] dataset_dir Path to the root directory that contains the dataset
/// \param[in] task Set the task type of reading voc data, now only support "Segmentation" or "Detection"
/// \param[in] usage The type of data list text file to be read (default = "train").
/// \param[in] class_indexing A str-to-int mapping from label name to index, only valid in "Detection" task
/// \param[in] decode Decode the images after reading
/// \param[in] sampler Object used to choose samples from the dataset. If sampler is not given,
///     a `RandomSampler` will be used to randomly iterate the entire dataset (default = RandomSampler())
/// \return Shared pointer to the current Dataset
std::shared_ptr<VOCNode> VOC(const std::string &dataset_dir, const std::string &task = "Segmentation",
                             const std::string &usage = "train",
                             const std::map<std::string, int32_t> &class_indexing = {}, bool decode = false,
                             const std::shared_ptr<SamplerObj> &sampler = RandomSampler());
#endif

/// \brief Function to create a ZipNode
/// \notes Applies zip to the dataset
/// \param[in] datasets List of shared pointers to the datasets that we want to zip
/// \return Shared pointer to the current Dataset
std::shared_ptr<ZipNode> Zip(const std::vector<std::shared_ptr<Dataset>> &datasets);

/// \class Dataset datasets.h
/// \brief A base class to represent a dataset in the data pipeline.
class Dataset : public std::enable_shared_from_this<Dataset> {
 public:
  // need friend class so they can access the children_ field
  friend class Iterator;
  friend class mindspore::dataset::TreeAdapter;

  /// \brief Constructor
  Dataset();

  /// \brief Destructor
  ~Dataset() = default;

  /// \brief Pure virtual function to convert a Dataset class into a runtime dataset object
  /// \return The list of shared pointers to the newly created DatasetOps
  virtual std::vector<std::shared_ptr<DatasetOp>> Build() = 0;

  /// \brief Pure virtual function for derived class to implement parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  virtual Status ValidateParams() = 0;

  /// \brief Setter function for runtime number of workers
  /// \param[in] num_workers The number of threads in this operator
  /// \return Shared pointer to the original object
  std::shared_ptr<Dataset> SetNumWorkers(int32_t num_workers) {
#if !defined(_WIN32) && !defined(_WIN64)
#ifndef ENABLE_ANDROID
    int32_t cpu_count = sysconf(_SC_NPROCESSORS_CONF);
    if (cpu_count < 0 || cpu_count > INT32_MAX) {
      MS_LOG(ERROR) << "Error determining current CPU: " << cpu_count;
      return nullptr;
    }
    if (num_workers < 1 || num_workers > cpu_count) {
      MS_LOG(ERROR) << "num_workers exceeds the boundary between 1 and " << cpu_count;
      return nullptr;
    }
#endif
#endif
    num_workers_ = num_workers;
    return shared_from_this();
  }

  /// \brief Function to create an Iterator over the Dataset pipeline
  /// \param[in] columns List of columns to be used to specify the order of columns
  /// \return Shared pointer to the Iterator
  std::shared_ptr<Iterator> CreateIterator(std::vector<std::string> columns = {});

  /// \brief Function to create a BatchNode
  /// \notes Combines batch_size number of consecutive rows into batches
  /// \param[in] batch_size Path to the root directory that contains the dataset
  /// \param[in] drop_remainder Determines whether or not to drop the last possibly incomplete
  ///     batch. If true, and if there are less than batch_size rows
  ///     available to make the last batch, then those rows will
  ///     be dropped and not propagated to the next node
  /// \return Shared pointer to the current BatchNode
  std::shared_ptr<BatchNode> Batch(int32_t batch_size, bool drop_remainder = false);

#ifndef ENABLE_ANDROID
  /// \brief Function to create a BucketBatchByLengthNode
  /// \notes Combines batch_size number of consecutive rows into batches
  /// \param[in] column_names Columns passed to element_length_function
  /// \param[in] bucket_boundaries A list consisting of the upper boundaries of the buckets.
  ///    Must be strictly increasing. If there are n boundaries, n+1 buckets are created: One bucket for
  ///    [0, bucket_boundaries[0]), one bucket for [bucket_boundaries[i], bucket_boundaries[i+1]) for each
  ///    0<i<n, and one bucket for [bucket_boundaries[n-1], inf).
  /// \param[in] bucket_batch_sizes A list consisting of the batch sizes for each bucket.
  ///    Must contain elements equal to the size of bucket_boundaries + 1.
  /// \param[in] element_length_function A function pointer that takes in TensorRow and outputs a TensorRow. The output
  ///    must contain a single tensor containing a single int32_t. If no value is provided, then size of column_names
  ///    must be 1, and the size of the first dimension of that column will be taken as the length (default=nullptr)
  /// \param[in] pad_info Represents how to batch each column. The key corresponds to the column name, the value must
  ///    be a tuple of 2 elements.  The first element corresponds to the shape to pad to, and the second element
  ///    corresponds to the value to pad with. If a column is not specified, then that column will be padded to the
  ///    longest in the current batch, and 0 will be used as the padding value. Any unspecified dimensions will be
  ///    padded to the longest in the current batch, unless if pad_to_bucket_boundary is true. If no padding is wanted,
  ///    set pad_info to None (default=empty dictionary).
  /// \param[in] pad_to_bucket_boundary If true, will pad each unspecified dimension in pad_info to the bucket_boundary
  ///    minus 1. If there are any elements that fall into the last bucket, an error will occur (default=false).
  /// \param[in] drop_remainder If true, will drop the last batch for each bucket if it is not a full batch
  ///    (default=false).
  /// \return Shared pointer to the current BucketBatchByLengthNode
  std::shared_ptr<BucketBatchByLengthNode> BucketBatchByLength(
    const std::vector<std::string> &column_names, const std::vector<int32_t> &bucket_boundaries,
    const std::vector<int32_t> &bucket_batch_sizes,
    std::function<TensorRow(TensorRow)> element_length_function = nullptr,
    const std::map<std::string, std::pair<TensorShape, std::shared_ptr<Tensor>>> &pad_info = {},
    bool pad_to_bucket_boundary = false, bool drop_remainder = false);

  /// \brief Function to create a Vocab from source dataset
  /// \notes Build a vocab from a dataset. This would collect all the unique words in a dataset and return a vocab
  ///    which contains top_k most frequent words (if top_k is specified)
  /// \param[in] columns Column names to get words from. It can be a vector of column names
  /// \param[in] freq_range A tuple of integers (min_frequency, max_frequency). Words within the frequency
  ///    range would be kept. 0 <= min_frequency <= max_frequency <= total_words. min_frequency/max_frequency
  ///    can be set to default, which corresponds to 0/total_words separately
  /// \param[in] top_k Number of words to be built into vocab. top_k most frequent words are
  ///    taken. The top_k is taken after freq_range. If not enough top_k, all words will be taken
  /// \param[in] special_tokens A list of strings, each one is a special token
  /// \param[in] special_first Whether special_tokens will be prepended/appended to vocab, If special_tokens
  ///    is specified and special_first is set to default, special_tokens will be prepended
  /// \return Shared pointer to the current Vocab
  std::shared_ptr<Vocab> BuildVocab(const std::vector<std::string> &columns = {},
                                    const std::pair<int64_t, int64_t> &freq_range = {0, kDeMaxFreq},
                                    int64_t top_k = kDeMaxTopk, const std::vector<std::string> &special_tokens = {},
                                    bool special_first = true);
#endif

  /// \brief Function to create a ConcatNode
  /// \notes Concat the datasets in the input
  /// \param[in] datasets List of shared pointers to the dataset that should be concatenated together
  /// \return Shared pointer to the current ConcatNode
  std::shared_ptr<ConcatNode> Concat(const std::vector<std::shared_ptr<Dataset>> &datasets);

  /// \brief Function to create a MapNode
  /// \notes Applies each operation in operations to this dataset
  /// \param[in] operations Vector of operations to be applied on the dataset. Operations are
  ///     applied in the order they appear in this list
  /// \param[in] input_columns Vector of the names of the columns that will be passed to the first
  ///     operation as input. The size of this list must match the number of
  ///     input columns expected by the first operator. The default input_columns
  ///     is the first column
  /// \param[in] output_columns Vector of names assigned to the columns outputted by the last operation
  ///     This parameter is mandatory if len(input_columns) != len(output_columns)
  ///     The size of this list must match the number of output columns of the
  ///     last operation. The default output_columns will have the same
  ///     name as the input columns, i.e., the columns will be replaced
  /// \param[in] project_columns A list of column names to project
  /// \return Shared pointer to the current MapNode
  std::shared_ptr<MapNode> Map(std::vector<std::shared_ptr<TensorOperation>> operations,
                               std::vector<std::string> input_columns = {},
                               std::vector<std::string> output_columns = {},
                               const std::vector<std::string> &project_columns = {});

  /// \brief Function to create a Project Dataset
  /// \notes Applies project to the dataset
  /// \param[in] columns The name of columns to project
  /// \return Shared pointer to the current Dataset
  std::shared_ptr<ProjectNode> Project(const std::vector<std::string> &columns);

  /// \brief Function to create a Rename Dataset
  /// \notes Renames the columns in the input dataset
  /// \param[in] input_columns List of the input columns to rename
  /// \param[in] output_columns List of the output columns
  /// \return Shared pointer to the current Dataset
  std::shared_ptr<RenameNode> Rename(const std::vector<std::string> &input_columns,
                                     const std::vector<std::string> &output_columns);

  /// \brief Function to create a RepeatNode
  /// \notes Repeats this dataset count times. Repeat indefinitely if count is -1
  /// \param[in] count Number of times the dataset should be repeated
  /// \return Shared pointer to the current Dataset
  /// \note Repeat will return shared pointer to `Dataset` instead of `RepeatNode`
  ///     due to a limitation in the current implementation
  std::shared_ptr<Dataset> Repeat(int32_t count = -1);

  /// \brief Function to create a Shuffle Dataset
  /// \notes Randomly shuffles the rows of this dataset
  /// \param[in] buffer_size The size of the buffer (must be larger than 1) for shuffling
  /// \return Shared pointer to the current ShuffleNode
  std::shared_ptr<ShuffleNode> Shuffle(int32_t buffer_size);

  /// \brief Function to create a SkipNode
  /// \notes Skips count elements in this dataset.
  /// \param[in] count Number of elements the dataset to be skipped.
  /// \return Shared pointer to the current SkipNode
  std::shared_ptr<SkipNode> Skip(int32_t count);

  /// \brief Function to create a TakeNode
  /// \notes Takes count elements in this dataset.
  /// \param[in] count Number of elements the dataset to be taken.
  /// \return Shared pointer to the current Dataset
  std::shared_ptr<Dataset> Take(int32_t count = -1);

  /// \brief Function to create a Zip Dataset
  /// \notes Applies zip to the dataset
  /// \param[in] datasets A list of shared pointers to the datasets that we want to zip
  /// \return Shared pointer to the current Dataset
  std::shared_ptr<ZipNode> Zip(const std::vector<std::shared_ptr<Dataset>> &datasets);

 protected:
  std::vector<std::shared_ptr<Dataset>> children;
  std::shared_ptr<Dataset> parent;

  int32_t num_workers_;
  int32_t rows_per_buffer_;
  int32_t connector_que_size_;
  int32_t worker_connector_size_;
};

class SchemaObj {
 public:
  /// \brief Constructor
  explicit SchemaObj(const std::string &schema_file = "");

  /// \brief Destructor
  ~SchemaObj() = default;

  /// \brief SchemaObj init function
  /// \return bool true if schema init success
  bool init();

  /// \brief Add new column to the schema
  /// \param[in] name name of the column.
  /// \param[in] de_type data type of the column(TypeId).
  /// \param[in] shape shape of the column.
  /// \return bool true if schema init success
  bool add_column(std::string name, TypeId de_type, std::vector<int32_t> shape);

  /// \brief Add new column to the schema
  /// \param[in] name name of the column.
  /// \param[in] de_type data type of the column(std::string).
  /// \param[in] shape shape of the column.
  /// \return bool true if schema init success
  bool add_column(std::string name, std::string de_type, std::vector<int32_t> shape);

  /// \brief Get a JSON string of the schema
  /// \return JSON string of the schema
  std::string to_json();

  /// \brief Get a JSON string of the schema
  std::string to_string() { return to_json(); }

  /// \brief set a new value to dataset_type
  inline void set_dataset_type(std::string dataset_type) { dataset_type_ = dataset_type; }

  /// \brief set a new value to num_rows
  inline void set_num_rows(int32_t num_rows) { num_rows_ = num_rows; }

  /// \brief get the current num_rows
  inline int32_t get_num_rows() { return num_rows_; }

 private:
  /// \brief Parse the columns and add it to columns
  /// \param[in] columns dataset attribution information, decoded from schema file.
  ///    support both nlohmann::json::value_t::array and nlohmann::json::value_t::onject.
  /// \return JSON string of the schema
  bool parse_column(nlohmann::json columns);

  /// \brief Get schema file from json file
  /// \param[in] json_obj object of json parsed.
  /// \return bool true if json dump success
  bool from_json(nlohmann::json json_obj);

  int32_t num_rows_;
  std::string dataset_type_;
  std::string schema_file_;
  nlohmann::json columns_;
};

/* ####################################### Derived Dataset classes ################################# */

// DERIVED DATASET CLASSES FOR LEAF-NODE DATASETS
// (In alphabetical order)

class AlbumNode : public Dataset {
 public:
  /// \brief Constructor
  AlbumNode(const std::string &dataset_dir, const std::string &data_schema,
            const std::vector<std::string> &column_names, bool decode, const std::shared_ptr<SamplerObj> &sampler);

  /// \brief Destructor
  ~AlbumNode() = default;

  /// \brief a base class override function to create a runtime dataset op object from this class
  /// \return shared pointer to the newly created DatasetOp
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  std::string schema_path_;
  std::vector<std::string> column_names_;
  bool decode_;
  std::shared_ptr<SamplerObj> sampler_;
};

class CelebANode : public Dataset {
 public:
  /// \brief Constructor
  CelebANode(const std::string &dataset_dir, const std::string &usage, const std::shared_ptr<SamplerObj> &sampler,
             const bool &decode, const std::set<std::string> &extensions);

  /// \brief Destructor
  ~CelebANode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return shared pointer to the list of newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  std::string usage_;
  bool decode_;
  std::set<std::string> extensions_;
  std::shared_ptr<SamplerObj> sampler_;
};
// DERIVED DATASET CLASSES FOR LEAF-NODE DATASETS
// (In alphabetical order)

class Cifar10Node : public Dataset {
 public:
  /// \brief Constructor
  Cifar10Node(const std::string &dataset_dir, const std::string &usage, std::shared_ptr<SamplerObj> sampler);

  /// \brief Destructor
  ~Cifar10Node() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  std::string usage_;
  std::shared_ptr<SamplerObj> sampler_;
};

class Cifar100Node : public Dataset {
 public:
  /// \brief Constructor
  Cifar100Node(const std::string &dataset_dir, const std::string &usage, std::shared_ptr<SamplerObj> sampler);

  /// \brief Destructor
  ~Cifar100Node() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  std::string usage_;
  std::shared_ptr<SamplerObj> sampler_;
};

/// \class CLUENode
/// \brief A Dataset derived class to represent CLUE dataset
class CLUENode : public Dataset {
 public:
  /// \brief Constructor
  CLUENode(const std::vector<std::string> dataset_files, std::string task, std::string usage, int64_t num_samples,
           ShuffleMode shuffle, int32_t num_shards, int32_t shard_id);

  /// \brief Destructor
  ~CLUENode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  /// \brief Split string based on a character delimiter
  /// \return A string vector
  std::vector<std::string> split(const std::string &s, char delim);

  std::vector<std::string> dataset_files_;
  std::string task_;
  std::string usage_;
  int64_t num_samples_;
  ShuffleMode shuffle_;
  int32_t num_shards_;
  int32_t shard_id_;
};

class CocoNode : public Dataset {
 public:
  /// \brief Constructor
  CocoNode(const std::string &dataset_dir, const std::string &annotation_file, const std::string &task,
           const bool &decode, const std::shared_ptr<SamplerObj> &sampler);

  /// \brief Destructor
  ~CocoNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return shared pointer to the list of newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  std::string annotation_file_;
  std::string task_;
  bool decode_;
  std::shared_ptr<SamplerObj> sampler_;
};

/// \brief Record type for CSV
enum CsvType : uint8_t { INT = 0, FLOAT, STRING };

/// \brief Base class of CSV Record
class CsvBase {
 public:
  CsvBase() = default;
  explicit CsvBase(CsvType t) : type(t) {}
  virtual ~CsvBase() {}
  CsvType type;
};

/// \brief CSV Record that can represent integer, float and string.
template <typename T>
class CsvRecord : public CsvBase {
 public:
  CsvRecord() = default;
  CsvRecord(CsvType t, T v) : CsvBase(t), value(v) {}
  ~CsvRecord() {}
  T value;
};

class CSVNode : public Dataset {
 public:
  /// \brief Constructor
  CSVNode(const std::vector<std::string> &dataset_files, char field_delim,
          const std::vector<std::shared_ptr<CsvBase>> &column_defaults, const std::vector<std::string> &column_names,
          int64_t num_samples, ShuffleMode shuffle, int32_t num_shards, int32_t shard_id);

  /// \brief Destructor
  ~CSVNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return shared pointer to the list of newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::string> dataset_files_;
  char field_delim_;
  std::vector<std::shared_ptr<CsvBase>> column_defaults_;
  std::vector<std::string> column_names_;
  int64_t num_samples_;
  ShuffleMode shuffle_;
  int32_t num_shards_;
  int32_t shard_id_;
};

/// \class ImageFolderNode
/// \brief A Dataset derived class to represent ImageFolder dataset
class ImageFolderNode : public Dataset {
 public:
  /// \brief Constructor
  ImageFolderNode(std::string dataset_dir, bool decode, std::shared_ptr<SamplerObj> sampler, bool recursive,
                  std::set<std::string> extensions, std::map<std::string, int32_t> class_indexing);

  /// \brief Destructor
  ~ImageFolderNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  bool decode_;
  bool recursive_;
  std::shared_ptr<SamplerObj> sampler_;
  std::map<std::string, int32_t> class_indexing_;
  std::set<std::string> exts_;
};

#ifndef ENABLE_ANDROID
class ManifestNode : public Dataset {
 public:
  /// \brief Constructor
  ManifestNode(const std::string &dataset_file, const std::string &usage, const std::shared_ptr<SamplerObj> &sampler,
               const std::map<std::string, int32_t> &class_indexing, bool decode);

  /// \brief Destructor
  ~ManifestNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_file_;
  std::string usage_;
  bool decode_;
  std::map<std::string, int32_t> class_index_;
  std::shared_ptr<SamplerObj> sampler_;
};
#endif

#ifndef ENABLE_ANDROID
class MindDataNode : public Dataset {
 public:
  /// \brief Constructor
  MindDataNode(const std::vector<std::string> &dataset_files, const std::vector<std::string> &columns_list,
               const std::shared_ptr<SamplerObj> &sampler, nlohmann::json padded_sample, int64_t num_padded);

  /// \brief Constructor
  MindDataNode(const std::string &dataset_file, const std::vector<std::string> &columns_list,
               const std::shared_ptr<SamplerObj> &sampler, nlohmann::json padded_sample, int64_t num_padded);

  /// \brief Destructor
  ~MindDataNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

  /// \brief Build sampler chain for minddata dataset
  /// \return Status Status::OK() if input sampler is valid
  Status BuildMindDatasetSamplerChain(const std::shared_ptr<SamplerObj> &sampler,
                                      std::vector<std::shared_ptr<mindrecord::ShardOperator>> *operators_,
                                      int64_t num_padded);

  /// \brief Set sample_bytes when padded_sample has py::byte value
  /// \note Pybind will use this function to set sample_bytes into MindDataNode
  void SetSampleBytes(std::map<std::string, std::string> *sample_bytes);

 private:
  std::string dataset_file_;                // search_for_pattern_ will be true in this mode
  std::vector<std::string> dataset_files_;  // search_for_pattern_ will be false in this mode
  bool search_for_pattern_;
  std::vector<std::string> columns_list_;
  std::shared_ptr<SamplerObj> sampler_;
  nlohmann::json padded_sample_;
  std::map<std::string, std::string> sample_bytes_;  // enable in python
  int64_t num_padded_;
};
#endif

class MnistNode : public Dataset {
 public:
  /// \brief Constructor
  MnistNode(std::string dataset_dir, std::string usage, std::shared_ptr<SamplerObj> sampler);

  /// \brief Destructor
  ~MnistNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::string dataset_dir_;
  std::string usage_;
  std::shared_ptr<SamplerObj> sampler_;
};

class RandomNode : public Dataset {
 public:
  // Some constants to provide limits to random generation.
  static constexpr int32_t kMaxNumColumns = 4;
  static constexpr int32_t kMaxRank = 4;
  static constexpr int32_t kMaxDimValue = 32;

  /// \brief Constructor
  RandomNode(const int32_t &total_rows, std::shared_ptr<SchemaObj> schema, const std::vector<std::string> &columns_list,
             const std::shared_ptr<SamplerObj> &sampler)
      : total_rows_(total_rows),
        schema_path_(""),
        schema_(std::move(schema)),
        columns_list_(columns_list),
        sampler_(std::move(sampler)) {}

  /// \brief Constructor
  RandomNode(const int32_t &total_rows, std::string schema_path, const std::vector<std::string> &columns_list,
             const std::shared_ptr<SamplerObj> &sampler)
      : total_rows_(total_rows), schema_path_(schema_path), columns_list_(columns_list), sampler_(std::move(sampler)) {}

  /// \brief Destructor
  ~RandomNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  /// \brief A quick inline for producing a random number between (and including) min/max
  /// \param[in] min minimum number that can be generated.
  /// \param[in] max maximum number that can be generated.
  /// \return The generated random number
  int32_t GenRandomInt(int32_t min, int32_t max);

  int32_t total_rows_;
  std::string schema_path_;
  std::shared_ptr<SchemaObj> schema_;
  std::vector<std::string> columns_list_;
  std::shared_ptr<SamplerObj> sampler_;
  std::mt19937 rand_gen_;
};

/// \class TextFileNode
/// \brief A Dataset derived class to represent TextFile dataset
class TextFileNode : public Dataset {
 public:
  /// \brief Constructor
  TextFileNode(std::vector<std::string> dataset_files, int32_t num_samples, ShuffleMode shuffle, int32_t num_shards,
               int32_t shard_id);

  /// \brief Destructor
  ~TextFileNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::string> dataset_files_;
  int32_t num_samples_;
  int32_t num_shards_;
  int32_t shard_id_;
  ShuffleMode shuffle_;
};

/// \class TFRecordNode
/// \brief A Dataset derived class to represent TFRecord dataset
class TFRecordNode : public Dataset {
 public:
  /// \brief Constructor
  /// \note Parameter 'schema' is the path to the schema file
  TFRecordNode(const std::vector<std::string> &dataset_files, std::string schema,
               const std::vector<std::string> &columns_list, int64_t num_samples, ShuffleMode shuffle,
               int32_t num_shards, int32_t shard_id, bool shard_equal_rows)
      : dataset_files_(dataset_files),
        schema_path_(schema),
        columns_list_(columns_list),
        num_samples_(num_samples),
        shuffle_(shuffle),
        num_shards_(num_shards),
        shard_id_(shard_id),
        shard_equal_rows_(shard_equal_rows) {}

  /// \brief Constructor
  /// \note Parameter 'schema' is shared pointer to Schema object
  TFRecordNode(const std::vector<std::string> &dataset_files, std::shared_ptr<SchemaObj> schema,
               const std::vector<std::string> &columns_list, int64_t num_samples, ShuffleMode shuffle,
               int32_t num_shards, int32_t shard_id, bool shard_equal_rows)
      : dataset_files_(dataset_files),
        schema_obj_(schema),
        columns_list_(columns_list),
        num_samples_(num_samples),
        shuffle_(shuffle),
        num_shards_(num_shards),
        shard_id_(shard_id),
        shard_equal_rows_(shard_equal_rows) {}

  /// \brief Destructor
  ~TFRecordNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::string> dataset_files_;
  std::string schema_path_;  // schema_path_ path to schema file. It is set when type of schema parameter is string
  std::shared_ptr<SchemaObj> schema_obj_;  // schema_obj_ schema object.
  std::vector<std::string> columns_list_;
  int64_t num_samples_;
  ShuffleMode shuffle_;
  int32_t num_shards_;
  int32_t shard_id_;
  bool shard_equal_rows_;
};

#ifndef ENABLE_ANDROID
class VOCNode : public Dataset {
 public:
  /// \brief Constructor
  VOCNode(const std::string &dataset_dir, const std::string &task, const std::string &usage,
          const std::map<std::string, int32_t> &class_indexing, bool decode, std::shared_ptr<SamplerObj> sampler);

  /// \brief Destructor
  ~VOCNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return shared pointer to the list of newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  const std::string kColumnImage = "image";
  const std::string kColumnTarget = "target";
  const std::string kColumnBbox = "bbox";
  const std::string kColumnLabel = "label";
  const std::string kColumnDifficult = "difficult";
  const std::string kColumnTruncate = "truncate";
  std::string dataset_dir_;
  std::string task_;
  std::string usage_;
  std::map<std::string, int32_t> class_index_;
  bool decode_;
  std::shared_ptr<SamplerObj> sampler_;
};
#endif

// DERIVED DATASET CLASSES FOR DATASET OPS
// (In alphabetical order)

class BatchNode : public Dataset {
 public:
  /// \brief Constructor
  BatchNode(std::shared_ptr<Dataset> child, int32_t batch_size, bool drop_remainder, bool pad,
            std::vector<std::string> cols_to_map,
            std::map<std::string, std::pair<TensorShape, std::shared_ptr<Tensor>>> pad_map);

  /// \brief Destructor
  ~BatchNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  int32_t batch_size_;
  bool drop_remainder_;
  bool pad_;
  std::vector<std::string> cols_to_map_;
  std::map<std::string, std::pair<TensorShape, std::shared_ptr<Tensor>>> pad_map_;
};

#ifndef ENABLE_ANDROID
class BucketBatchByLengthNode : public Dataset {
 public:
  /// \brief Constructor
  BucketBatchByLengthNode(std::shared_ptr<Dataset> child, const std::vector<std::string> &column_names,
                          const std::vector<int32_t> &bucket_boundaries, const std::vector<int32_t> &bucket_batch_sizes,
                          std::function<TensorRow(TensorRow)> element_length_function = nullptr,
                          const std::map<std::string, std::pair<TensorShape, std::shared_ptr<Tensor>>> &pad_info = {},
                          bool pad_to_bucket_boundary = false, bool drop_remainder = false);

  /// \brief Destructor
  ~BucketBatchByLengthNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::string> column_names_;
  std::vector<int32_t> bucket_boundaries_;
  std::vector<int32_t> bucket_batch_sizes_;
  std::function<TensorRow(TensorRow)> element_length_function_;
  std::map<std::string, std::pair<TensorShape, std::shared_ptr<Tensor>>> pad_info_;
  bool pad_to_bucket_boundary_;
  bool drop_remainder_;
};

class BuildVocabNode : public Dataset {
 public:
  /// \brief Constructor
  BuildVocabNode(std::shared_ptr<Dataset> child, std::shared_ptr<Vocab> vocab, const std::vector<std::string> &columns,
                 const std::pair<int64_t, int64_t> &freq_range, int64_t top_k,
                 const std::vector<std::string> &special_tokens, bool special_first);

  /// \brief Destructor
  ~BuildVocabNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::shared_ptr<Vocab> vocab_;
  std::vector<std::string> columns_;
  std::pair<int64_t, int64_t> freq_range_;
  int64_t top_k_;
  std::vector<std::string> special_tokens_;
  bool special_first_;
};
#endif

class ConcatNode : public Dataset {
 public:
  /// \brief Constructor
  explicit ConcatNode(const std::vector<std::shared_ptr<Dataset>> &datasets);

  /// \brief Destructor
  ~ConcatNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::shared_ptr<Dataset>> datasets_;
};

class MapNode : public Dataset {
 public:
  /// \brief Constructor
  MapNode(std::shared_ptr<Dataset> child, std::vector<std::shared_ptr<TensorOperation>> operations,
          std::vector<std::string> input_columns = {}, std::vector<std::string> output_columns = {},
          const std::vector<std::string> &columns = {});

  /// \brief Destructor
  ~MapNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::shared_ptr<TensorOperation>> operations_;
  std::vector<std::string> input_columns_;
  std::vector<std::string> output_columns_;
  std::vector<std::string> project_columns_;
};

class ProjectNode : public Dataset {
 public:
  /// \brief Constructor
  explicit ProjectNode(std::shared_ptr<Dataset> child, const std::vector<std::string> &columns);

  /// \brief Destructor
  ~ProjectNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::string> columns_;
};

class RenameNode : public Dataset {
 public:
  /// \brief Constructor
  explicit RenameNode(std::shared_ptr<Dataset> child, const std::vector<std::string> &input_columns,
                      const std::vector<std::string> &output_columns);

  /// \brief Destructor
  ~RenameNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::string> input_columns_;
  std::vector<std::string> output_columns_;
};

class RepeatNode : public Dataset {
 public:
  /// \brief Constructor
  explicit RepeatNode(std::shared_ptr<Dataset> child, int32_t count);

  /// \brief Destructor
  ~RepeatNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  int32_t repeat_count_;
};

class ShuffleNode : public Dataset {
 public:
  ShuffleNode(std::shared_ptr<Dataset> child, int32_t shuffle_size, bool reset_every_epoch);

  ~ShuffleNode() = default;

  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  Status ValidateParams() override;

 private:
  int32_t shuffle_size_;
  uint32_t shuffle_seed_;
  bool reset_every_epoch_;
};

class SkipNode : public Dataset {
 public:
  /// \brief Constructor
  explicit SkipNode(std::shared_ptr<Dataset> child, int32_t count);

  /// \brief Destructor
  ~SkipNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  int32_t skip_count_;
};

class TakeNode : public Dataset {
 public:
  /// \brief Constructor
  explicit TakeNode(std::shared_ptr<Dataset> child, int32_t count);

  /// \brief Destructor
  ~TakeNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return shared pointer to the list of newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  int32_t take_count_;
};

class ZipNode : public Dataset {
 public:
  /// \brief Constructor
  explicit ZipNode(const std::vector<std::shared_ptr<Dataset>> &datasets);

  /// \brief Destructor
  ~ZipNode() = default;

  /// \brief a base class override function to create the required runtime dataset op objects for this class
  /// \return The list of shared pointers to the newly created DatasetOps
  std::vector<std::shared_ptr<DatasetOp>> Build() override;

  /// \brief Parameters validation
  /// \return Status Status::OK() if all the parameters are valid
  Status ValidateParams() override;

 private:
  std::vector<std::shared_ptr<Dataset>> datasets_;
};

}  // namespace api
}  // namespace dataset
}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_MINDDATA_DATASET_INCLUDE_DATASETS_H_
