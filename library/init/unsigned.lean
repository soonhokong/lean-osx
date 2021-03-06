/-
Copyright (c) 2016 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Leonardo de Moura
-/
prelude
import init.fin

open nat
definition unsigned_sz : nat := succ 4294967295

attribute [reducible]
definition unsigned := fin unsigned_sz

namespace unsigned
/- We cannot use tactic dec_trivial here because the tactic framework has not been defined yet. -/
private lemma zero_lt_unsigned_sz : 0 < unsigned_sz :=
zero_lt_succ _

definition of_nat (n : nat) : unsigned :=
if H : n < unsigned_sz then fin.mk n H else fin.mk 0 zero_lt_unsigned_sz

definition to_nat (c : unsigned) : nat :=
fin.val c
end unsigned

attribute [instance]
definition unsigned.has_decidable_eq : decidable_eq unsigned :=
have decidable_eq (fin unsigned_sz), from fin.has_decidable_eq _,
this
