# Remaining assignment requirements

Source: [CS3241 Assignment 4: TWISTED](cs3241_assgt4.pdf). This checklist is based on the requirements, rubric, and current [starter code](../main.cpp).

The six objective rubric categories below total **80%**. Completing this checklist supports those categories; it does not guarantee a grade. The entire **Draw Objects** task is deferred, including object design, repeated placement, tangent/normal alignment, and the `O` toggle. Artistic appreciation, beauty, novelty, and artistic rankings are excluded. Submitting a saved drawing remains in scope.

All boxes start unchecked. Feature sections describe remaining capabilities; the integration section includes verification of existing capabilities. Integration edge cases are acceptance checks, not additional requirements explicitly stated in the PDF.

## Control lines — 5%

- [ ] Consecutive control points are connected by green control lines.
- [ ] Newly added control points are included in the control lines.
- [ ] `L` hides the control lines.
- [ ] `L` restores the control lines.

## Clear all — 5%

- [ ] `E` removes all control points.
- [ ] Clearing removes all geometry derived from the removed points.
- [ ] Clearing an empty drawing works.
- [ ] Repeated add-and-clear cycles work.
- [ ] New points can be added normally after clearing.

## Bézier curves — 20%

- [ ] Four control points produce a cubic Bézier curve.
- [ ] Each curve starts at its first control point.
- [ ] Each curve ends at its fourth control point.
- [ ] Successive curves use overlapping groups: points 1–4, 4–7, 7–10, and so on.
- [ ] Adjacent curves meet at their shared endpoint.
- [ ] Each curve is approximated using `NLINESEGMENT` straight segments.
- [ ] Curve positions are calculated without predefined curve evaluators or curve-generation libraries.

## C1 continuity — 20%

- [ ] `C` enables C1 continuity.
- [ ] The first curve remains unchanged when continuity is enabled.
- [ ] Only the second control point of each subsequent curve is adjusted.
- [ ] Adjacent curves have matching tangent directions at every shared endpoint where the derivatives are nonzero.
- [ ] Adjacent curves have matching derivative magnitudes at every shared endpoint.
- [ ] Adjusted second control points appear red when control points are visible.
- [ ] Their original positions appear grey when control points are visible.
- [ ] Control lines follow the adjusted control points.
- [ ] Curves follow the adjusted control points.
- [ ] Disabling C1 continuity restores the original control-point configuration.
- [ ] Repeated continuity toggles preserve the original configuration.

## Tangent vectors — 20%

- [ ] `T` displays tangent arrows.
- [ ] `T` hides tangent arrows.
- [ ] Each complete curve displays `NOBJECTONCURVE` tangent arrows at positions with defined tangent directions.
- [ ] Tangent arrows originate at positions on their corresponding curve.
- [ ] Tangent arrows align with the curve's local tangent.
- [ ] Tangent arrows point in the direction of increasing curve parameter.
- [ ] Tangent arrows reflect the active continuity mode.

## Integration and existing-capability verification

### Curve completion and continuity

- [ ] Fewer than four points produce no Bézier curve.
- [ ] An incomplete final group does not produce an additional curve.
- [ ] Existing complete curves remain correct while another group is being entered.
- [ ] Adding points works with C1 continuity enabled.
- [ ] Control-line visibility works in both continuity modes.
- [ ] Control-point visibility works in both continuity modes.
- [ ] Tangent visibility works in both continuity modes.
- [ ] Clearing works in both continuity modes.
- [ ] Coincident control points do not cause crashes or invalid rendering.

### Existing input and persistence capabilities

- [ ] Left-clicking continues to add control points in input order.
- [ ] `P` continues to hide control points.
- [ ] `P` continues to restore control points.
- [ ] `W` saves the current control-point configuration.
- [ ] `R` restores a saved control-point configuration.
- [ ] Saving and reloading preserves the original configuration after continuity toggling.
- [ ] Saving works in both continuity modes.
- [ ] Loading works in both continuity modes.

## Meaningful additions and compilation — 10%

- [ ] The completed assignment contains meaningful additions beyond the starter code.
- [ ] The completed program compiles successfully in the supplied grading project.
- [ ] The completed program launches successfully.

## Drawing and submission

- [ ] A Bézier-curve drawing is saved for submission.
- [ ] The submitted drawing can be loaded successfully.
- [ ] The final `main.cpp` is included in the submission.
- [ ] The drawing data file is included under the required submission filename.

**Filename discrepancy:** The PDF's drawing instructions and the starter code use `savefile.txt`, while its submission paragraph says `save.txt`. Use `savefile.txt` as the working filename and confirm the required submission filename before submitting.

**Optional grader notes:** If there are extra features that graders should notice, the PDF asks for an explanation at the top of `main.cpp`. Extra features are not required by this checklist.
