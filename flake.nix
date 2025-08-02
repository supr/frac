{
  description = "A C++ SDL application to generate fractals";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/?ref=nixos-unstable";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = { self, nixpkgs, nixgl, ... }:
    let
      supportedSystems = ["x86_64-linux"];

      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems(system: f {
        pkgs = import nixpkgs { 
          inherit system;
          overlays = [ nixgl.overlay ];
        };
      });

    in
    {
      packages = forEachSupportedSystem({ pkgs }: {
        default = pkgs.stdenv.mkDerivation {
          pname = "frac";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
          ];

          # buildInputs = with pkgs; [
          #   sdl3
          # ];
          buildInputs = [
            pkgs.sdl3
            pkgs.nixgl.nixGLIntel
          ];
        };
      });

      devShells = forEachSupportedSystem({ pkgs }: {
        default = pkgs.mkShell {
          # buildInputs = with pkgs; [
          #   cmake
          #   pkg-config
          #   sdl3
          #   gcc15
          #   gdb
          #   nixgl.nixGLIntel
          # ];
          buildInputs = [
            pkgs.cmake
            pkgs.pkg-config
            pkgs.sdl3
            pkgs.gcc15
            pkgs.gdb
            pkgs.nixgl.nixGLIntel
          ];
        };
      });
    };
}
