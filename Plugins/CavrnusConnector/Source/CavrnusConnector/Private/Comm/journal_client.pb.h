// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: journal_client.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_journal_5fclient_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_journal_5fclient_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021006 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/timestamp.pb.h>
#include "journal_operations_client.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_journal_5fclient_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_journal_5fclient_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_journal_5fclient_2eproto;
namespace Journal {
class Journal;
struct JournalDefaultTypeInternal;
extern JournalDefaultTypeInternal _Journal_default_instance_;
class JournalEntry;
struct JournalEntryDefaultTypeInternal;
extern JournalEntryDefaultTypeInternal _JournalEntry_default_instance_;
class OperationCollection;
struct OperationCollectionDefaultTypeInternal;
extern OperationCollectionDefaultTypeInternal _OperationCollection_default_instance_;
}  // namespace Journal
PROTOBUF_NAMESPACE_OPEN
template<> ::Journal::Journal* Arena::CreateMaybeMessage<::Journal::Journal>(Arena*);
template<> ::Journal::JournalEntry* Arena::CreateMaybeMessage<::Journal::JournalEntry>(Arena*);
template<> ::Journal::OperationCollection* Arena::CreateMaybeMessage<::Journal::OperationCollection>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace Journal {

// ===================================================================

class JournalEntry final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Journal.JournalEntry) */ {
 public:
  inline JournalEntry() : JournalEntry(nullptr) {}
  ~JournalEntry() override;
  explicit PROTOBUF_CONSTEXPR JournalEntry(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  JournalEntry(const JournalEntry& from);
  JournalEntry(JournalEntry&& from) noexcept
    : JournalEntry() {
    *this = ::std::move(from);
  }

  inline JournalEntry& operator=(const JournalEntry& from) {
    CopyFrom(from);
    return *this;
  }
  inline JournalEntry& operator=(JournalEntry&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const JournalEntry& default_instance() {
    return *internal_default_instance();
  }
  static inline const JournalEntry* internal_default_instance() {
    return reinterpret_cast<const JournalEntry*>(
               &_JournalEntry_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(JournalEntry& a, JournalEntry& b) {
    a.Swap(&b);
  }
  inline void Swap(JournalEntry* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(JournalEntry* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  JournalEntry* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<JournalEntry>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const JournalEntry& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const JournalEntry& from) {
    JournalEntry::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(JournalEntry* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Journal.JournalEntry";
  }
  protected:
  explicit JournalEntry(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kConnectionIdFieldNumber = 3,
    kConsumedTransientUniqueIdFieldNumber = 5,
    kIdFieldNumber = 6,
    kTimeFieldNumber = 2,
    kOpFieldNumber = 4,
  };
  // optional string connectionId = 3;
  bool has_connectionid() const;
  private:
  bool _internal_has_connectionid() const;
  public:
  void clear_connectionid();
  const std::string& connectionid() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_connectionid(ArgT0&& arg0, ArgT... args);
  std::string* mutable_connectionid();
  PROTOBUF_NODISCARD std::string* release_connectionid();
  void set_allocated_connectionid(std::string* connectionid);
  private:
  const std::string& _internal_connectionid() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_connectionid(const std::string& value);
  std::string* _internal_mutable_connectionid();
  public:

  // optional string consumedTransientUniqueId = 5;
  bool has_consumedtransientuniqueid() const;
  private:
  bool _internal_has_consumedtransientuniqueid() const;
  public:
  void clear_consumedtransientuniqueid();
  const std::string& consumedtransientuniqueid() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_consumedtransientuniqueid(ArgT0&& arg0, ArgT... args);
  std::string* mutable_consumedtransientuniqueid();
  PROTOBUF_NODISCARD std::string* release_consumedtransientuniqueid();
  void set_allocated_consumedtransientuniqueid(std::string* consumedtransientuniqueid);
  private:
  const std::string& _internal_consumedtransientuniqueid() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_consumedtransientuniqueid(const std::string& value);
  std::string* _internal_mutable_consumedtransientuniqueid();
  public:

  // string id = 6;
  void clear_id();
  const std::string& id() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_id(ArgT0&& arg0, ArgT... args);
  std::string* mutable_id();
  PROTOBUF_NODISCARD std::string* release_id();
  void set_allocated_id(std::string* id);
  private:
  const std::string& _internal_id() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_id(const std::string& value);
  std::string* _internal_mutable_id();
  public:

  // .google.protobuf.Timestamp time = 2;
  bool has_time() const;
  private:
  bool _internal_has_time() const;
  public:
  void clear_time();
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& time() const;
  PROTOBUF_NODISCARD ::PROTOBUF_NAMESPACE_ID::Timestamp* release_time();
  ::PROTOBUF_NAMESPACE_ID::Timestamp* mutable_time();
  void set_allocated_time(::PROTOBUF_NAMESPACE_ID::Timestamp* time);
  private:
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& _internal_time() const;
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _internal_mutable_time();
  public:
  void unsafe_arena_set_allocated_time(
      ::PROTOBUF_NAMESPACE_ID::Timestamp* time);
  ::PROTOBUF_NAMESPACE_ID::Timestamp* unsafe_arena_release_time();

  // .Journal.Operation op = 4;
  bool has_op() const;
  private:
  bool _internal_has_op() const;
  public:
  void clear_op();
  const ::Journal::Operation& op() const;
  PROTOBUF_NODISCARD ::Journal::Operation* release_op();
  ::Journal::Operation* mutable_op();
  void set_allocated_op(::Journal::Operation* op);
  private:
  const ::Journal::Operation& _internal_op() const;
  ::Journal::Operation* _internal_mutable_op();
  public:
  void unsafe_arena_set_allocated_op(
      ::Journal::Operation* op);
  ::Journal::Operation* unsafe_arena_release_op();

  // @@protoc_insertion_point(class_scope:Journal.JournalEntry)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr connectionid_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr consumedtransientuniqueid_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr id_;
    ::PROTOBUF_NAMESPACE_ID::Timestamp* time_;
    ::Journal::Operation* op_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_journal_5fclient_2eproto;
};
// -------------------------------------------------------------------

class Journal final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Journal.Journal) */ {
 public:
  inline Journal() : Journal(nullptr) {}
  ~Journal() override;
  explicit PROTOBUF_CONSTEXPR Journal(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Journal(const Journal& from);
  Journal(Journal&& from) noexcept
    : Journal() {
    *this = ::std::move(from);
  }

  inline Journal& operator=(const Journal& from) {
    CopyFrom(from);
    return *this;
  }
  inline Journal& operator=(Journal&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Journal& default_instance() {
    return *internal_default_instance();
  }
  static inline const Journal* internal_default_instance() {
    return reinterpret_cast<const Journal*>(
               &_Journal_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Journal& a, Journal& b) {
    a.Swap(&b);
  }
  inline void Swap(Journal* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Journal* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Journal* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Journal>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Journal& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const Journal& from) {
    Journal::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Journal* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Journal.Journal";
  }
  protected:
  explicit Journal(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kEntriesFieldNumber = 1,
  };
  // repeated .Journal.JournalEntry entries = 1;
  int entries_size() const;
  private:
  int _internal_entries_size() const;
  public:
  void clear_entries();
  ::Journal::JournalEntry* mutable_entries(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::JournalEntry >*
      mutable_entries();
  private:
  const ::Journal::JournalEntry& _internal_entries(int index) const;
  ::Journal::JournalEntry* _internal_add_entries();
  public:
  const ::Journal::JournalEntry& entries(int index) const;
  ::Journal::JournalEntry* add_entries();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::JournalEntry >&
      entries() const;

  // @@protoc_insertion_point(class_scope:Journal.Journal)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::JournalEntry > entries_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_journal_5fclient_2eproto;
};
// -------------------------------------------------------------------

class OperationCollection final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Journal.OperationCollection) */ {
 public:
  inline OperationCollection() : OperationCollection(nullptr) {}
  ~OperationCollection() override;
  explicit PROTOBUF_CONSTEXPR OperationCollection(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  OperationCollection(const OperationCollection& from);
  OperationCollection(OperationCollection&& from) noexcept
    : OperationCollection() {
    *this = ::std::move(from);
  }

  inline OperationCollection& operator=(const OperationCollection& from) {
    CopyFrom(from);
    return *this;
  }
  inline OperationCollection& operator=(OperationCollection&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const OperationCollection& default_instance() {
    return *internal_default_instance();
  }
  static inline const OperationCollection* internal_default_instance() {
    return reinterpret_cast<const OperationCollection*>(
               &_OperationCollection_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(OperationCollection& a, OperationCollection& b) {
    a.Swap(&b);
  }
  inline void Swap(OperationCollection* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(OperationCollection* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  OperationCollection* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<OperationCollection>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const OperationCollection& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const OperationCollection& from) {
    OperationCollection::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(OperationCollection* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Journal.OperationCollection";
  }
  protected:
  explicit OperationCollection(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kOperationsFieldNumber = 1,
  };
  // repeated .Journal.Operation operations = 1;
  int operations_size() const;
  private:
  int _internal_operations_size() const;
  public:
  void clear_operations();
  ::Journal::Operation* mutable_operations(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::Operation >*
      mutable_operations();
  private:
  const ::Journal::Operation& _internal_operations(int index) const;
  ::Journal::Operation* _internal_add_operations();
  public:
  const ::Journal::Operation& operations(int index) const;
  ::Journal::Operation* add_operations();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::Operation >&
      operations() const;

  // @@protoc_insertion_point(class_scope:Journal.OperationCollection)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::Operation > operations_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_journal_5fclient_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// JournalEntry

// .google.protobuf.Timestamp time = 2;
inline bool JournalEntry::_internal_has_time() const {
  return this != internal_default_instance() && _impl_.time_ != nullptr;
}
inline bool JournalEntry::has_time() const {
  return _internal_has_time();
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& JournalEntry::_internal_time() const {
  const ::PROTOBUF_NAMESPACE_ID::Timestamp* p = _impl_.time_;
  return p != nullptr ? *p : reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Timestamp&>(
      ::PROTOBUF_NAMESPACE_ID::_Timestamp_default_instance_);
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& JournalEntry::time() const {
  // @@protoc_insertion_point(field_get:Journal.JournalEntry.time)
  return _internal_time();
}
inline void JournalEntry::unsafe_arena_set_allocated_time(
    ::PROTOBUF_NAMESPACE_ID::Timestamp* time) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.time_);
  }
  _impl_.time_ = time;
  if (time) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:Journal.JournalEntry.time)
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* JournalEntry::release_time() {
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = _impl_.time_;
  _impl_.time_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* JournalEntry::unsafe_arena_release_time() {
  // @@protoc_insertion_point(field_release:Journal.JournalEntry.time)
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = _impl_.time_;
  _impl_.time_ = nullptr;
  return temp;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* JournalEntry::_internal_mutable_time() {
  
  if (_impl_.time_ == nullptr) {
    auto* p = CreateMaybeMessage<::PROTOBUF_NAMESPACE_ID::Timestamp>(GetArenaForAllocation());
    _impl_.time_ = p;
  }
  return _impl_.time_;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* JournalEntry::mutable_time() {
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _msg = _internal_mutable_time();
  // @@protoc_insertion_point(field_mutable:Journal.JournalEntry.time)
  return _msg;
}
inline void JournalEntry::set_allocated_time(::PROTOBUF_NAMESPACE_ID::Timestamp* time) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.time_);
  }
  if (time) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(time));
    if (message_arena != submessage_arena) {
      time = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, time, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.time_ = time;
  // @@protoc_insertion_point(field_set_allocated:Journal.JournalEntry.time)
}

// optional string connectionId = 3;
inline bool JournalEntry::_internal_has_connectionid() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool JournalEntry::has_connectionid() const {
  return _internal_has_connectionid();
}
inline void JournalEntry::clear_connectionid() {
  _impl_.connectionid_.ClearToEmpty();
  _impl_._has_bits_[0] &= ~0x00000001u;
}
inline const std::string& JournalEntry::connectionid() const {
  // @@protoc_insertion_point(field_get:Journal.JournalEntry.connectionId)
  return _internal_connectionid();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void JournalEntry::set_connectionid(ArgT0&& arg0, ArgT... args) {
 _impl_._has_bits_[0] |= 0x00000001u;
 _impl_.connectionid_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Journal.JournalEntry.connectionId)
}
inline std::string* JournalEntry::mutable_connectionid() {
  std::string* _s = _internal_mutable_connectionid();
  // @@protoc_insertion_point(field_mutable:Journal.JournalEntry.connectionId)
  return _s;
}
inline const std::string& JournalEntry::_internal_connectionid() const {
  return _impl_.connectionid_.Get();
}
inline void JournalEntry::_internal_set_connectionid(const std::string& value) {
  _impl_._has_bits_[0] |= 0x00000001u;
  _impl_.connectionid_.Set(value, GetArenaForAllocation());
}
inline std::string* JournalEntry::_internal_mutable_connectionid() {
  _impl_._has_bits_[0] |= 0x00000001u;
  return _impl_.connectionid_.Mutable(GetArenaForAllocation());
}
inline std::string* JournalEntry::release_connectionid() {
  // @@protoc_insertion_point(field_release:Journal.JournalEntry.connectionId)
  if (!_internal_has_connectionid()) {
    return nullptr;
  }
  _impl_._has_bits_[0] &= ~0x00000001u;
  auto* p = _impl_.connectionid_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.connectionid_.IsDefault()) {
    _impl_.connectionid_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void JournalEntry::set_allocated_connectionid(std::string* connectionid) {
  if (connectionid != nullptr) {
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  _impl_.connectionid_.SetAllocated(connectionid, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.connectionid_.IsDefault()) {
    _impl_.connectionid_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Journal.JournalEntry.connectionId)
}

// .Journal.Operation op = 4;
inline bool JournalEntry::_internal_has_op() const {
  return this != internal_default_instance() && _impl_.op_ != nullptr;
}
inline bool JournalEntry::has_op() const {
  return _internal_has_op();
}
inline const ::Journal::Operation& JournalEntry::_internal_op() const {
  const ::Journal::Operation* p = _impl_.op_;
  return p != nullptr ? *p : reinterpret_cast<const ::Journal::Operation&>(
      ::Journal::_Operation_default_instance_);
}
inline const ::Journal::Operation& JournalEntry::op() const {
  // @@protoc_insertion_point(field_get:Journal.JournalEntry.op)
  return _internal_op();
}
inline void JournalEntry::unsafe_arena_set_allocated_op(
    ::Journal::Operation* op) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.op_);
  }
  _impl_.op_ = op;
  if (op) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:Journal.JournalEntry.op)
}
inline ::Journal::Operation* JournalEntry::release_op() {
  
  ::Journal::Operation* temp = _impl_.op_;
  _impl_.op_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::Journal::Operation* JournalEntry::unsafe_arena_release_op() {
  // @@protoc_insertion_point(field_release:Journal.JournalEntry.op)
  
  ::Journal::Operation* temp = _impl_.op_;
  _impl_.op_ = nullptr;
  return temp;
}
inline ::Journal::Operation* JournalEntry::_internal_mutable_op() {
  
  if (_impl_.op_ == nullptr) {
    auto* p = CreateMaybeMessage<::Journal::Operation>(GetArenaForAllocation());
    _impl_.op_ = p;
  }
  return _impl_.op_;
}
inline ::Journal::Operation* JournalEntry::mutable_op() {
  ::Journal::Operation* _msg = _internal_mutable_op();
  // @@protoc_insertion_point(field_mutable:Journal.JournalEntry.op)
  return _msg;
}
inline void JournalEntry::set_allocated_op(::Journal::Operation* op) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.op_);
  }
  if (op) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(op));
    if (message_arena != submessage_arena) {
      op = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, op, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.op_ = op;
  // @@protoc_insertion_point(field_set_allocated:Journal.JournalEntry.op)
}

// optional string consumedTransientUniqueId = 5;
inline bool JournalEntry::_internal_has_consumedtransientuniqueid() const {
  bool value = (_impl_._has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool JournalEntry::has_consumedtransientuniqueid() const {
  return _internal_has_consumedtransientuniqueid();
}
inline void JournalEntry::clear_consumedtransientuniqueid() {
  _impl_.consumedtransientuniqueid_.ClearToEmpty();
  _impl_._has_bits_[0] &= ~0x00000002u;
}
inline const std::string& JournalEntry::consumedtransientuniqueid() const {
  // @@protoc_insertion_point(field_get:Journal.JournalEntry.consumedTransientUniqueId)
  return _internal_consumedtransientuniqueid();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void JournalEntry::set_consumedtransientuniqueid(ArgT0&& arg0, ArgT... args) {
 _impl_._has_bits_[0] |= 0x00000002u;
 _impl_.consumedtransientuniqueid_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Journal.JournalEntry.consumedTransientUniqueId)
}
inline std::string* JournalEntry::mutable_consumedtransientuniqueid() {
  std::string* _s = _internal_mutable_consumedtransientuniqueid();
  // @@protoc_insertion_point(field_mutable:Journal.JournalEntry.consumedTransientUniqueId)
  return _s;
}
inline const std::string& JournalEntry::_internal_consumedtransientuniqueid() const {
  return _impl_.consumedtransientuniqueid_.Get();
}
inline void JournalEntry::_internal_set_consumedtransientuniqueid(const std::string& value) {
  _impl_._has_bits_[0] |= 0x00000002u;
  _impl_.consumedtransientuniqueid_.Set(value, GetArenaForAllocation());
}
inline std::string* JournalEntry::_internal_mutable_consumedtransientuniqueid() {
  _impl_._has_bits_[0] |= 0x00000002u;
  return _impl_.consumedtransientuniqueid_.Mutable(GetArenaForAllocation());
}
inline std::string* JournalEntry::release_consumedtransientuniqueid() {
  // @@protoc_insertion_point(field_release:Journal.JournalEntry.consumedTransientUniqueId)
  if (!_internal_has_consumedtransientuniqueid()) {
    return nullptr;
  }
  _impl_._has_bits_[0] &= ~0x00000002u;
  auto* p = _impl_.consumedtransientuniqueid_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.consumedtransientuniqueid_.IsDefault()) {
    _impl_.consumedtransientuniqueid_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void JournalEntry::set_allocated_consumedtransientuniqueid(std::string* consumedtransientuniqueid) {
  if (consumedtransientuniqueid != nullptr) {
    _impl_._has_bits_[0] |= 0x00000002u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000002u;
  }
  _impl_.consumedtransientuniqueid_.SetAllocated(consumedtransientuniqueid, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.consumedtransientuniqueid_.IsDefault()) {
    _impl_.consumedtransientuniqueid_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Journal.JournalEntry.consumedTransientUniqueId)
}

// string id = 6;
inline void JournalEntry::clear_id() {
  _impl_.id_.ClearToEmpty();
}
inline const std::string& JournalEntry::id() const {
  // @@protoc_insertion_point(field_get:Journal.JournalEntry.id)
  return _internal_id();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void JournalEntry::set_id(ArgT0&& arg0, ArgT... args) {
 
 _impl_.id_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Journal.JournalEntry.id)
}
inline std::string* JournalEntry::mutable_id() {
  std::string* _s = _internal_mutable_id();
  // @@protoc_insertion_point(field_mutable:Journal.JournalEntry.id)
  return _s;
}
inline const std::string& JournalEntry::_internal_id() const {
  return _impl_.id_.Get();
}
inline void JournalEntry::_internal_set_id(const std::string& value) {
  
  _impl_.id_.Set(value, GetArenaForAllocation());
}
inline std::string* JournalEntry::_internal_mutable_id() {
  
  return _impl_.id_.Mutable(GetArenaForAllocation());
}
inline std::string* JournalEntry::release_id() {
  // @@protoc_insertion_point(field_release:Journal.JournalEntry.id)
  return _impl_.id_.Release();
}
inline void JournalEntry::set_allocated_id(std::string* id) {
  if (id != nullptr) {
    
  } else {
    
  }
  _impl_.id_.SetAllocated(id, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.id_.IsDefault()) {
    _impl_.id_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Journal.JournalEntry.id)
}

// -------------------------------------------------------------------

// Journal

// repeated .Journal.JournalEntry entries = 1;
inline int Journal::_internal_entries_size() const {
  return _impl_.entries_.size();
}
inline int Journal::entries_size() const {
  return _internal_entries_size();
}
inline void Journal::clear_entries() {
  _impl_.entries_.Clear();
}
inline ::Journal::JournalEntry* Journal::mutable_entries(int index) {
  // @@protoc_insertion_point(field_mutable:Journal.Journal.entries)
  return _impl_.entries_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::JournalEntry >*
Journal::mutable_entries() {
  // @@protoc_insertion_point(field_mutable_list:Journal.Journal.entries)
  return &_impl_.entries_;
}
inline const ::Journal::JournalEntry& Journal::_internal_entries(int index) const {
  return _impl_.entries_.Get(index);
}
inline const ::Journal::JournalEntry& Journal::entries(int index) const {
  // @@protoc_insertion_point(field_get:Journal.Journal.entries)
  return _internal_entries(index);
}
inline ::Journal::JournalEntry* Journal::_internal_add_entries() {
  return _impl_.entries_.Add();
}
inline ::Journal::JournalEntry* Journal::add_entries() {
  ::Journal::JournalEntry* _add = _internal_add_entries();
  // @@protoc_insertion_point(field_add:Journal.Journal.entries)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::JournalEntry >&
Journal::entries() const {
  // @@protoc_insertion_point(field_list:Journal.Journal.entries)
  return _impl_.entries_;
}

// -------------------------------------------------------------------

// OperationCollection

// repeated .Journal.Operation operations = 1;
inline int OperationCollection::_internal_operations_size() const {
  return _impl_.operations_.size();
}
inline int OperationCollection::operations_size() const {
  return _internal_operations_size();
}
inline ::Journal::Operation* OperationCollection::mutable_operations(int index) {
  // @@protoc_insertion_point(field_mutable:Journal.OperationCollection.operations)
  return _impl_.operations_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::Operation >*
OperationCollection::mutable_operations() {
  // @@protoc_insertion_point(field_mutable_list:Journal.OperationCollection.operations)
  return &_impl_.operations_;
}
inline const ::Journal::Operation& OperationCollection::_internal_operations(int index) const {
  return _impl_.operations_.Get(index);
}
inline const ::Journal::Operation& OperationCollection::operations(int index) const {
  // @@protoc_insertion_point(field_get:Journal.OperationCollection.operations)
  return _internal_operations(index);
}
inline ::Journal::Operation* OperationCollection::_internal_add_operations() {
  return _impl_.operations_.Add();
}
inline ::Journal::Operation* OperationCollection::add_operations() {
  ::Journal::Operation* _add = _internal_add_operations();
  // @@protoc_insertion_point(field_add:Journal.OperationCollection.operations)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Journal::Operation >&
OperationCollection::operations() const {
  // @@protoc_insertion_point(field_list:Journal.OperationCollection.operations)
  return _impl_.operations_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace Journal

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_journal_5fclient_2eproto
