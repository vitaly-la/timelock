{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/release-24.11";
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      solver = pkgs.rustPlatform.buildRustPackage {
        name = "solver";
        src = rust/solver;

        nativeBuildInputs = with pkgs; [ cargo m4 ];

        cargoLock.lockFile = rust/solver/Cargo.lock;
      };
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        name = "timelock";
        src = ./.;

        nativeBuildInputs = [ pkgs.makeWrapper ];
        buildInputs = [ (pkgs.python313.withPackages (ps: [ ps.pycryptodome ])) ];

        postInstall = ''
          mkdir -p $out/{lib,bin}
          cp python/timelock.py $out/bin/timelock
          chmod +x $out/bin/timelock
          cp ${solver}/bin/solver $out/lib
          wrapProgram $out/bin/timelock --set SOLVER $out/lib/solver
        '';
      };
    };
}
