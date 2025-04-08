{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, flake-utils, nixpkgs }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        gmpNative = pkgs.gmp.overrideAttrs (old: {
          depsBuildBuild = [ pkgs.gcc ];

          NIX_ENFORCE_NO_NATIVE = false;
          CFLAGS = "-march=native -O3";
          CXXFLAGS = "-march=native -O3";
          NIX_CFLAGS_COMPILE = "-march=native -O3";
        });
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          name = "timelock";
          src = ./.;

          nativeBuildInputs = [ pkgs.makeWrapper ];
          buildInputs = with pkgs; [ (python313.withPackages (ps: with ps; [ pycryptodome progress ])) gcc gmpNative ];

          NIX_ENFORCE_NO_NATIVE = false;

          buildPhase = ''
            gcc -march=native -O3 solver.c -lgmp -o solver
          '';

          installPhase = ''
            mkdir -p $out/{bin,lib}
            cp timelock.py $out/bin/timelock
            chmod +x $out/bin/timelock
            cp solver $out/lib
            wrapProgram $out/bin/timelock --set SOLVER $out/lib/solver
          '';
        };
      }
    );
}
