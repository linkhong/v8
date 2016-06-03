// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INTERPRETER_BYTECODE_PEEPHOLE_OPTIMIZER_H_
#define V8_INTERPRETER_BYTECODE_PEEPHOLE_OPTIMIZER_H_

#include "src/interpreter/bytecode-pipeline.h"

namespace v8 {
namespace internal {
namespace interpreter {

class ConstantArrayBuilder;

// An optimization stage for performing peephole optimizations on
// generated bytecode. The optimizer may buffer one bytecode
// internally.
class BytecodePeepholeOptimizer final : public BytecodePipelineStage,
                                        public ZoneObject {
 public:
  BytecodePeepholeOptimizer(ConstantArrayBuilder* constant_array_builder,
                            BytecodePipelineStage* next_stage);

  // BytecodePipelineStage interface.
  void Write(BytecodeNode* node) override;
  void WriteJump(BytecodeNode* node, BytecodeLabel* label) override;
  void BindLabel(BytecodeLabel* label) override;
  void BindLabel(const BytecodeLabel& target, BytecodeLabel* label) override;
  Handle<BytecodeArray> ToBytecodeArray(
      int fixed_register_count, int parameter_count,
      Handle<FixedArray> handler_table) override;

 private:
  BytecodeNode* OptimizeAndEmitLast(BytecodeNode* current);
  BytecodeNode* Optimize(BytecodeNode* current);
  void Flush();

  void TryToRemoveLastExpressionPosition(const BytecodeNode* const current);
  bool TransformLastAndCurrentBytecodes(BytecodeNode* const current);
  bool CanElideCurrent(const BytecodeNode* const current) const;
  bool CanElideLast(const BytecodeNode* const current) const;
  bool CanElideLastBasedOnSourcePosition(
      const BytecodeNode* const current) const;

  // Simple substitution methods.
  bool RemoveToBooleanFromJump(BytecodeNode* const current);
  bool RemoveToBooleanFromLogicalNot(BytecodeNode* const current);
  bool ChangeLdaToLdr(BytecodeNode* const current);

  void InvalidateLast();
  bool LastIsValid() const;
  void SetLast(const BytecodeNode* const node);

  bool LastBytecodePutsNameInAccumulator() const;

  Handle<Object> GetConstantForIndexOperand(const BytecodeNode* const node,
                                            int index) const;

  ConstantArrayBuilder* constant_array_builder_;
  BytecodePipelineStage* next_stage_;
  BytecodeNode last_;

  DISALLOW_COPY_AND_ASSIGN(BytecodePeepholeOptimizer);
};

}  // namespace interpreter
}  // namespace internal
}  // namespace v8

#endif  // V8_INTERPRETER_BYTECODE_PEEPHOLE_OPTIMIZER_H_
