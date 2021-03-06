/-
Copyright (c) 2014 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Leonardo de Moura
-/
prelude
import init.bool

namespace pos_num
  protected definition mul (a b : pos_num) : pos_num :=
  pos_num.rec_on a
    b
    (λn r, bit0 r + b)
    (λn r, bit0 r)

  definition lt (a b : pos_num) : bool :=
  pos_num.rec_on a
    (λ b, pos_num.cases_on b
      ff
      (λm, tt)
      (λm, tt))
    (λn f b, pos_num.cases_on b
      ff
      (λm, f m)
      (λm, f m))
    (λn f b, pos_num.cases_on b
      ff
      (λm, f (succ m))
      (λm, f m))
    b

  definition le (a b : pos_num) : bool :=
  pos_num.lt a (succ b)
end pos_num

attribute [instance]
definition pos_num_has_mul : has_mul pos_num :=
⟨pos_num.mul⟩

namespace num
  open pos_num

  definition pred (a : num) : num :=
  num.rec_on a zero (λp, bool.cond (is_one p) zero (pos (pred p)))

  definition size (a : num) : num :=
  num.rec_on a (pos pos_num.one) (λp, pos (size p))

  protected definition mul (a b : num) : num :=
  num.rec_on a zero (λpa, num.rec_on b zero (λpb, pos (pos_num.mul pa pb)))
end num

attribute [instance]
definition num_has_mul : has_mul num :=
⟨num.mul⟩

namespace num
  protected definition le (a b : num) : bool :=
  num.rec_on a tt (λpa, num.rec_on b ff (λpb, pos_num.le pa pb))

  private definition psub (a b : pos_num) : num :=
  pos_num.rec_on a
    (λb, zero)
    (λn f b,
      bool.cond (pos_num.le (bit1 n) b)
        zero
        (pos_num.cases_on b
           (pos (bit0 n))
           (λm, 2 * f m)
           (λm, 2 * f m + 1)))
    (λn f b,
      bool.cond (pos_num.le (bit0 n) b)
        zero
        (pos_num.cases_on b
           (pos (pos_num.pred (bit0 n)))
           (λm, pred (2 * f m))
           (λm, 2 * f m)))
    b

  protected definition sub (a b : num) : num :=
  num.rec_on a zero (λpa, num.rec_on b a (λpb, psub pa pb))
end num

attribute [instance]
definition num_has_sub : has_sub num :=
⟨num.sub⟩
