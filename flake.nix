{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/release-24.11";
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        name = "timelock";
        src = ./.;

        nativeBuildInputs = [ pkgs.makeWrapper ];
        buildInputs = with pkgs; [ (python313.withPackages (ps: with ps; [ pycryptodome progress ])) gcc gmp ];

        buildPhase = ''
          gcc -march=native -O3 solver.c -lgmp -o solver
        '';

        installPhase = ''
          mkdir -p $out/{lib,bin}
          cp timelock.py $out/bin/timelock
          chmod +x $out/bin/timelock
          cp solver $out/lib
          wrapProgram $out/bin/timelock --set SOLVER $out/lib/solver
        '';
      };
    };
}
