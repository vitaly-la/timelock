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
        buildInputs = [ (pkgs.python312.withPackages (ps: [ ps.pycryptodome ps.progress ])) ];
        installPhase = ''
          mkdir -p $out/bin
          cp main.py $out/bin/timelock
          chmod +x $out/bin/timelock
        '';
      };
    };
}
