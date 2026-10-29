# Assignment requirements and verification

Source: [CS3241 Assignment 4: TWISTED](cs3241_assgt4.pdf). Audited on 29 October 2026 against the current [implementation](../main.cpp) and `savefile.txt`.

The six objective rubric categories below total **80%**. Completing this checklist supports those categories; it does not guarantee a grade. **Draw Objects** is also implemented and verified below, although the PDF does not allocate it a separate rubric percentage. Artistic appreciation, beauty, novelty, and artistic rankings remain subjective. Submission packaging is still outstanding.

Checked boxes record source inspection, numerical checks, OpenGL feedback checks, or simulated input and rendered-output checks under Linux/Xvfb. Compilation and startup are also verified on all ten configured CI jobs. Integration edge cases are acceptance checks, not additional requirements explicitly stated in the PDF.

## Control lines — 5%

- [x] Consecutive control points are connected by green control lines.
- [x] Newly added control points are included in the control lines.
- [x] `L` hides the control lines.
- [x] `L` restores the control lines.

## Clear all — 5%

- [x] `E` removes all control points.
- [x] Clearing removes all geometry derived from the removed points.
- [x] Clearing an empty drawing works.
- [x] Repeated add-and-clear cycles work.
- [x] New points can be added normally after clearing.

## Bézier curves — 20%

- [x] Four control points produce a cubic Bézier curve.
- [x] Each curve starts at its first control point.
- [x] Each curve ends at its fourth control point.
- [x] Successive curves use overlapping groups: points 1–4, 4–7, 7–10, and so on.
- [x] Adjacent curves meet at their shared endpoint.
- [x] Each curve is approximated using `NLINESEGMENT` straight segments.
- [x] Curve positions are calculated without predefined curve evaluators or curve-generation libraries.

## C1 continuity — 20%

- [x] `C` enables C1 continuity.
- [x] The first curve remains unchanged when continuity is enabled.
- [x] Only the second control point of each subsequent curve is adjusted.
- [x] Adjacent curves have matching tangent directions at every shared endpoint where the derivatives are nonzero.
- [x] Adjacent curves have matching derivative magnitudes at every shared endpoint.
- [x] Adjusted second control points appear red when control points are visible.
- [x] Their original positions appear grey when control points are visible.
- [x] Control lines follow the adjusted control points.
- [x] Curves follow the adjusted control points.
- [x] Disabling C1 continuity restores the original control-point configuration.
- [x] Repeated continuity toggles preserve the original configuration.

## Tangent vectors — 20%

- [x] `T` displays tangent arrows.
- [x] `T` hides tangent arrows.
- [x] Each complete curve displays `NOBJECTONCURVE` tangent arrows at positions with defined tangent directions.
- [x] Tangent arrows originate at positions on their corresponding curve.
- [x] Tangent arrows align with the curve's local tangent.
- [x] Tangent arrows point in the direction of increasing curve parameter.
- [x] Tangent arrows reflect the active continuity mode.

## Draw objects

- [x] `O` displays and hides the cranes independently of the other display options.
- [x] Each complete curve draws `NOBJECTONCURVE` cranes at samples with defined tangent directions.
- [x] Cranes are placed on the curve using its tangent and perpendicular normal as local axes.
- [x] Placement follows the active continuity mode.
- [x] Object vertices and planar surface normals are specified explicitly.
- [x] No predefined object drawing helpers are used.
- [x] Clearing removes the cranes along with the rest of the drawing.
- [x] The crane design contains none of the prohibited themes listed in the PDF.
- [x] Additional requested behavior: coral/ivory colors are randomized at startup and remain stable during the session.

## Integration and existing-capability verification

### Curve completion and continuity

- [x] Fewer than four points produce no Bézier curve.
- [x] An incomplete final group does not produce an additional curve.
- [x] Existing complete curves remain correct while another group is being entered.
- [x] Adding points works with C1 continuity enabled.
- [x] Control-line visibility works in both continuity modes.
- [x] Control-point visibility works in both continuity modes.
- [x] Tangent visibility works in both continuity modes.
- [x] Clearing works in both continuity modes.
- [x] Coincident control points do not cause crashes or invalid rendering.

### Existing input and persistence capabilities

- [x] Left-clicking continues to add control points in input order.
- [x] `P` continues to hide control points.
- [x] `P` continues to restore control points.
- [x] `W` saves the current control-point configuration.
- [x] `R` restores a saved control-point configuration.
- [x] Saving and reloading preserves the original configuration after continuity toggling.
- [x] Saving works in both continuity modes.
- [x] Loading works in both continuity modes.

## Meaningful additions and compilation — 10%

- [x] The completed assignment contains meaningful additions beyond the starter code.
- [x] The completed program compiles successfully in the supplied grading project.
- [x] The completed program launches successfully.

## Drawing and submission

- [x] A Bézier-curve drawing is saved for submission.
- [x] The saved drawing can be loaded successfully.
- [ ] The final `main.cpp` is included in the submission.
- [ ] The drawing data file is included under the required submission filename.

**Filename discrepancy:** The PDF's drawing instructions and the starter code use `savefile.txt`, while its submission paragraph says `save.txt`. Use `savefile.txt` as the working filename and confirm the required submission filename before submitting.

The current `savefile.txt` contains **10 control points forming 3 complete cubic curves**, with no incomplete trailing group. It was loaded and rendered in both continuity modes, and temporary-file round trips preserved the original coordinates. The user's file was not modified. It is versioned and must also be included when preparing the submission.

**Optional grader notes:** If there are extra features that graders should notice, the PDF asks for an explanation at the top of `main.cpp`. Extra features are not required by this checklist.

## Verification evidence and limits

- The audited `main.cpp` has SHA-256 `e91497c5409d3963931f0acbc1cf7e3cc0e0b2f5192c0e3e7ba7273943cd24e8` and matches the exact source tested by [GitHub Actions run 40811259125](https://github.com/iZUMi-kyouka/cs3241_assgt4/actions/runs/40811259125).
- All ten CI jobs passed: Debug and Release on Windows x64/x86, macOS Intel/Apple Silicon, and Linux. All four downloaded MSBuild logs report **0 warnings and 0 errors** with the professor's original compiler settings retained. Local GCC and Clang Debug/Release checks also passed with warnings treated as errors.
- Actual curve drawing was checked for exactly `NLINESEGMENT` segments, correct endpoint sharing, and incomplete-group handling in both continuity modes. C1 derivatives and original-point preservation were checked across point counts from 0 through `MAXPTNO`.
- Tangent derivatives were compared with finite differences. OpenGL feedback checked arrow counts, direction, length, crane frame transforms, and graphics-state restoration. Input/rendering checks covered `C`, `E`, `L`, `O`, `P`, `T`, and original-only `R`/`W` round trips in both continuity modes.
- Tangents and cranes skip samples where the derivative is zero (or has squared length at most `1e-12`), because no tangent frame is defined there. Regular curves have the specified number of samples. The PDF does not prescribe handling for stationary samples.
- Inspection found manual Bernstein curve evaluation and explicitly authored crane vertices/normals; no OpenGL curve evaluators, predefined object helpers, or De Casteljau subdivision are used.
- Previews of the user's drawing are available as ignored build artifacts: `build/assignment-drawing-original.png` and `build/assignment-drawing-c1.png`. The drawing's artistic quality and the PDF's compactness/cleverness preference cannot be certified by functional tests.
- No submission archive was prepared or uploaded. The filename discrepancy above remains unresolved. Optional extra-feature notes are not currently present at the top of `main.cpp`.
