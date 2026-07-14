# Multiplayer Arena

A small third-person multiplayer arena game created in Unreal Engine 5.8.

The game supports listen-server LAN sessions for up to four players and includes a full lobby, match and post-match flow. It was created from the Unreal Engine Third Person C++ template.

Gameplay rules and replicated state are implemented in C++. Blueprints are used for configuration, UI, animations and visual effects.

## Features

* Hosting, finding and joining LAN sessions using `OnlineSubsystemNull`
* Replicated lobby ready state
* Match start controlled by the host
* Server-authoritative hitscan weapon
* Replicated health, kills, deaths and match time
* Death and respawn system
* Replicated health pickups
* Match result screen and return to lobby
* Previous-round summary displayed in the lobby

## Opening the project

1. Clone the repository.
2. Install Unreal Engine 5.8.
3. Right-click `MultiplayerArena.uproject` and select **Generate Visual Studio project files**.
4. Build the `MultiplayerArenaEditor` target using the `Development Editor` and `Win64` configuration.
5. Open `MultiplayerArena.uproject`.

The default map is `Lvl_Lobby`. Both `Lvl_Lobby` and `Lvl_Arena` are included in the project packaging settings.

## Testing a LAN game

Packaged builds provide the most reliable way to test LAN sessions. All computers should be connected to the same local network and use the same build.

1. Start the game on the host computer and select **Host**.
2. Start the game on another computer.
3. Select **Find Session**, then **Join**.
4. Each player selects **Ready** in the lobby.
5. Once at least two players are connected and everyone is ready, the host can start the match.

Windows Firewall may ask for permission when the game is started for the first time. The game should be allowed on private networks.

The game can also be tested locally using separate Standalone Game instances. PIE is useful for quick replication testing, but LAN discovery is best tested using packaged builds.

## Controls

| Input              | Action |
| ------------------ | ------ |
| `W`, `A`, `S`, `D` | Move   |
| Mouse              | Look   |
| `Space`            | Jump   |
| Left mouse button  | Fire   |

Lobby and menu actions are controlled through the UI.

## Game flow

1. The host creates a LAN session and opens the lobby.
2. Other players find the session and join.
3. Players mark themselves as ready.
4. The host starts the match once all players are ready.
5. The server travels to `Lvl_Arena`.
6. Players fight until the match timer reaches zero.
7. The server calculates the winner and displays the result.
8. After a short delay, all players return to the lobby.

The default match duration is 120 seconds. Players respawn after 3 seconds, and the match result is displayed for 8 seconds. These values can be changed in the Arena GameMode Blueprint.

## Main classes

| Class                           | Responsibility                                             |
| ------------------------------- | ---------------------------------------------------------- |
| `UMultiplayerSessionSubsystem`  | Creates, finds and joins LAN sessions                      |
| `ALobbyGameMode`                | Handles lobby rules and starts the match                   |
| `ALobbyGameState`               | Notifies the UI when the lobby player list changes         |
| `ALobbyPlayerState`             | Stores and replicates player readiness                     |
| `AArenaGameMode`                | Handles match timing, deaths, respawning and match results |
| `AArenaGameState`               | Replicates match time, match state and round result        |
| `AMultiplayerArenaPlayerState`  | Stores replicated kills and deaths                         |
| `AArenaCharacter`               | Handles health and weapon firing                           |
| `AHealthPickup`                 | Handles pickup collection and respawning                   |
| `UMultiplayerArenaGameInstance` | Stores the previous-round summary during map travel        |

GameMode classes contain server-only gameplay rules. GameState and PlayerState classes contain replicated data required by clients. PlayerControllers create UI only for their owning local player.

## Networking

* Character movement uses Unreal Engine's replicated `CharacterMovementComponent`.
* The game uses a listen server, which is sufficient for a small LAN-based arena game.
* Health, readiness, statistics, match state and pickup state are controlled by the server.
* Shooting is requested using a server RPC. The server validates the fire rate, performs the traces and applies damage.
* The weapon first traces from the camera to determine the aim point, then traces from the muzzle towards that point.
* Weapon visual and audio effects are sent using an unreliable multicast because they do not affect gameplay.
* Replicated properties use `OnRep` callbacks and delegates to update the UI.

## C++ and Blueprint configuration

The main gameplay values are exposed to Blueprints:

* minimum lobby player count
* match duration
* respawn delay
* result-screen duration
* weapon damage, range and fire interval
* weapon effects and sound
* maximum health
* pickup healing amount and respawn time

Blueprints are used for UI layout, character assets, animation setup, Niagara effects and pickup visuals.

## Optimization

The following optimizations were added:

* Health pickups have actor tick disabled.
* Pickups do not replicate movement and use a lower network update frequency.
* `ForceNetUpdate` is called when pickup state changes.
* Cosmetic weapon effects use an unreliable multicast.
* HUD and lobby widgets are updated through events instead of polling every frame.
* Character meshes use visibility-based animation ticking and update-rate optimization.
* Match timing and respawning use timers instead of actor tick.
