# AI & Systems Integration Plan — Godot list → UE 5.8 → Mistspire

Mistspire is an **Unreal Engine 5.8** PCVR game (`game/Mistspire.uproject`). The
audited list is Godot tooling; this document maps every item to its **UE 5.8
equivalent**, the **Mistspire hook**, and the **delivery status** in this
repository.

Legend:

| Status | Meaning |
|--------|---------|
| ✅ Engine-builtin | Available in UE 5.8 out of the box; nothing to install |
| 🔧 C++ implemented | Added in this PR under `game/Source/Mistspire/AI/` |
| 📐 Editor-asset | Author the asset in Unreal Editor (blueprints/behavior trees are binary `.uasset`) |
| 🧭 Design seam | Interface + local implementation provided; external backend is a drop-in swap |

---

## 1. Animation — Character animation blending / state machines

- **Godot:** `AnimationTree`, `AnimationPlayer`
- **UE 5.8 equivalent:** Animation Blueprint (`.uasset`) with **AnimGraph** +
  Anim **State Machine**, layered per limb (upper body mantling, lower body
  locomotion), driven by `AMistspireVRPawn` replicated state (`bIsClimbing`,
  `bGliderActive`, `bGrappleActive`).
- **Mistspire hook:** `AMistspireVRPawn` already exposes the replicated
  traversal booleans in `MistspireVRPawn.h`. Anim BP reads them directly.
- **Status:** ✅ Engine-builtin + 📐 Editor-asset (Anim BP asset authored in
  editor, reads existing pawn state).

## 2. AI / Game AI

### 2.1 Reinforcement learning for agents

- **Godot:** Godot RL Agents
- **UE 5.8 equivalent:** UE has no bundled RL trainer; the standard pattern is a
  **feature exporter + offline training loop** (train with Python/PyTorch, run
  inference in the game, optionally via a remote inference plugin).
- **Mistspire hook:** `UMistspireObservationRecorder` (🔧 implemented) samples
  a fixed observation vector (altitude, oxygen, stamina, weather, pressure,
  exposure, zone, beacon distance, nearest ghost) and appends CSV rows to
  `Saved/RL/observations.csv`. Training can treat the row as `[state]`, and the
  debug console actions (`mistspire.TeleportUp`, `mistspire.SetWeather`,
  `mistspire.RefillSurvival`) are the "reset" actions of an episode.
- **Status:** 🔧 C++ implemented (telemetry pipeline). Training env is out of
  scope for a PCVR shipping build.

### 2.2 Behavior trees

- **Godot:** Beehave
- **UE 5.8 equivalent:** Built-in **Behavior Tree** + **Blackboard** +
  `UBehaviorTreeComponent` (AIModule). Assets (BT, Blackboard) are `.uasset`.
- **Mistspire hook:** `AMistspireAIController` + C++ task/condition nodes in
  `MistspireBehaviorTreeNodes.h` (climb to altitude, seek shelter, refill
  oxygen, move to beacon, play dialogue line; conditions `IsLowOxygen`,
  `IsExposed`, `IsAtGoal`). Build a BT graph in-editor that assembles these
  nodes.
- **Status:** 🔧 C++ implemented (nodes + controller) + 📐 Editor-asset (BT graph).

### 2.3 Behavior trees + state machines

- **Godot:** LimboAI
- **UE 5.8 equivalent:** The engine **StateTree** plugin (UE 5.4+) composes
  states + tasks with a debugging UI; for logic that must live in C++ only,
  `UMistspireStateMachineComponent` (🔧) is a lightweight generic state machine.
- **Mistspire hook:** `UMistspireStateMachineComponent` drives weather, zone,
  and companion-orb behaviors; narrative transitions fire on state change.
- **Status:** 🔧 C++ implemented (generic FSM) + ✅ Engine-builtin (StateTree
  available in-editor for designers).

### 2.4 GOAP — goal-oriented action planning

- **Godot:** Godot GOAP
- **UE 5.8 equivalent:** No built-in GOAP. Implemented a self-contained planner.
- **Mistspire hook:** `UMistspireGOAPPlanner` in `MistspireGOAP.h` — A* over a
  predicate/value world state. Pre-wired actions: `ClimbHigher`, `SeekShelter`,
  `RefillOxygen`, `ReachBeacon`, `RestoreStamina`. Any actor can ask for a plan
  each decision tick and execute it.
- **Status:** 🔧 C++ implemented.

### 2.5 Utility AI — score-based NPC decisions

- **Godot:** Utility AI frameworks
- **UE 5.8 equivalent:** Built-in **Utility AI** module (UE 5.4+,
  `UUtilityAIComponent`). For C++-driven decision making this PR ships
  `UMistspireUtilityEvaluator` with curve considerations (🔧).
- **Mistspire hook:** `UMistspireUtilityEvaluator` scores decisions such as
  *Rest, Climb On, Find Shelter* from live survival inputs (`GetOxygenPercent`,
  `GetStaminaPercent`, weather, exposure) and exposes the winning decision to
  BTs, the FSM, and the debug console.
- **Status:** 🔧 C++ implemented + ✅ Engine-builtin (Utility AI module usable
  in-editor).

## 3. Multiplayer / Networking

- **Godot:** Nakama, Ludus
- **UE 5.8 equivalent:** UE replication + `OnlineSubsystem`. Nakama ships an
  official **Nakama UE SDK** (C++/Blueprint) and **Ludus** is an alternative
  engine; for Mistspire the durable piece is the leaderboard/avatar seam.
- **Mistspire hook:** `UMistspireLeaderboardService` (🔧) is a game-instance
  service that backs `AMistspireGameState::NotifyAltitudeSample` and
  `BroadcastSocialAchievement`. Backend is pluggable: `None` (local SaveGame
  persistence, shipping default for single-player PCVR) or `Steam` via
  `IOnlineLeaderboards` (already an OnlineSubsystemSteam dependency in
  `Mistspire.Build.cs`). Swapping in Nakama means implementing one interface.
- **Status:** 🔧 C++ implemented (seam + local backend) + 🧭 Design seam
  (Nakama/Steam drop-in).

## 4. ECS / Architecture

- **Godot:** Godex, Godot ECS
- **UE 5.8 equivalent:** **Mass Entity** framework (MassEntity + MassActors +
  MassSpawner, engine plugin).
- **Mistspire hook:** For entity densities that don't justify the Mass stack,
  `UMistspireEntitySubsystem` (🔧) is an archetype/component store: spawn
  entities with an archetype tag and component payload, query by archetype each
  tick, iterate without per-actor GC cost. Ghost pillars and pickups register
  here.
- **Status:** 🔧 C++ implemented (ECS-lite) + ✅ Engine-builtin (Mass available
  in-editor for large crowds).

## 5. Dialogue / Narrative

- **Godot:** Dialogic, Dialogue Manager
- **UE 5.8 equivalent:** Data-driven dialogue — `UDataTable` of row structs +
  a world subsystem conversation queue. Third-party editor tools (NotYetGames
  Dialogue, Flow Graph) sit on top of the same pattern.
- **Mistspire hook:** `UMistspireDialogueSubsystem` (🔧) plays `FDialogueLine`
  rows (speaker, text, duration, optional audio cue) with a dynamic
  `OnDialogueLine` delegate, and integrates with `UMistspireNarrativeSubsystem`
  so story beats can be queued from an optional editor-authored DataTable at
  `/Game/Data/DT_MistspireDialogue` (falls back to the built-in line set when
  the asset is absent).
- **Status:** 🔧 C++ implemented + 📐 Editor-asset (DataTable authored in
  editor; a built-in in-code line set ships for immediate use).

## 6. Procedural / Game systems

### 6.1 Steering behaviors

- **Godot:** Godot Steering
- **UE 5.8 equivalent:** No single built-in component; UE offers
  `ACrowdFollowingManager` (Detour crowd) for navmesh crowds and manual
  steering for free-floating actors.
- **Mistspire hook:** `UMistspireSteeringComponent` (🔧) implements seek /
  arrive / flee / separation / wander for free-flying actors. `AMistspireGuideSpirit`
  uses it to arrive at the player shoulder instead of raw `VInterpTo`.
- **Status:** 🔧 C++ implemented.

### 6.2 State charts

- **Godot:** Godot State Charts
- **UE 5.8 equivalent:** StateTree (above) and the engine Gameplay Framework
  state hierarchy; `UMistspireStateMachineComponent` (🔧) is the generic chart
  for gameplay actors.
- **Status:** 🔧 C++ implemented.

### 6.3 Behavior tree libraries

- **Godot:** Behavior Tree libraries
- **UE 5.8 equivalent:** Built-in Behavior Tree + Blackboard (2.2).
- **Status:** ✅ Engine-builtin + 🔧 C++ nodes.

---

## Implementation map (this PR)

| File | Delivers |
|------|----------|
| `AI/MistspireAITypes.h` | shared enums, world-state struct, delegates |
| `AI/MistspireGOAP.h/.cpp` | planner + Mistspire action library |
| `AI/MistspireUtilityAI.h/.cpp` | considerations + evaluator |
| `AI/MistspireStateMachine.h/.cpp` | generic state chart component |
| `AI/MistspireSteering.h/.cpp` | seek/arrive/flee/separation/wander |
| `AI/MistspireBehaviorTreeNodes.h/.cpp` | BT tasks + conditions |
| `AI/MistspireAIController.h/.cpp` | AI controller driving the stack |
| `MistspireDialogueSubsystem.h/.cpp` | data-driven dialogue + narrative hook |
| `MistspireEntitySubsystem.h/.cpp` | ECS-lite archetype store |
| `MistspireObservationRecorder.h/.cpp` | RL observation CSV pipeline |
| `MistspireLeaderboardService.h/.cpp` | networking/leaderboard seam |

## Debug console

| Command | Purpose |
|---------|---------|
| `mistspire.AIThink` | Run one utility-AI + GOAP decision for the AI controller |
| `mistspire.GOAPPlan <goal>` | Print the plan for a goal predicate |
| `mistspire.Speak <id>` | Queue a dialogue line from the built-in line set |
| `mistspire.ObservationStart` / `mistspire.ObservationStop` | Toggle RL observation CSV |
| `mistspire.SpawnGhostSim` | Spawn a steering-simulated ghost for testing |
| `mistspire.StateMachineDebug` | Log active FSM states of registered components |