{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {

      devShells.${system}.default =
        let
          llvmPkgs = pkgs.llvmPackages_22;
        in
        pkgs.mkShell.override
          {
            stdenv = llvmPkgs.stdenv;
          }
          {
            name = "rohrkabel-dev-shell";

            nativeBuildInputs = with pkgs; [
              gcc
              cmake
              ninja
              pkg-config
              clang-tools
              llvmPkgs.lldb
            ];

            buildInputs = with pkgs; [
              gcc.cc.lib
              pipewire
            ];
          };
    };
}
