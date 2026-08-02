{
  description = "student registration system";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              gnumake
              pkg-config
              gcc16
              nodejs
            ];

            buildInputs = with pkgs; [
              crow
              nlohmann_json
              libpqxx
              openssl
              libsodium
            ];

            shellHook = "unset TEMP TMP TEMPDIR TMPDIR";
          };
        });
    };
}
