// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20; //Specifies the liscense and solidity compiler version

import "@openzeppelin/contracts/token/ERC721/extensions/ERC721URIStorage.sol"; //Allows us to manage the URI tokens
import "@openzeppelin/contracts/access/Ownable.sol"; //Allows us to provide authorization and ownership to a specific account

contract POAP is ERC721URIStorage, Ownable { //Defines the POAP contract
    uint256 private _tokenIdCounter; //Keeps track of the number of tokens created

    constructor() ERC721("POAP", "POAP") Ownable(0x06c32EEEFC4CE25392909cf675508CaE9b646C53) { //Constructs initial contract

        _tokenIdCounter = 0; //Initializes the token counter to zero

    }

    function mintPOAP(address recipient, string memory tokenURI) external onlyOwner {//Mints an NFT for the POAP

        uint256 tokenId = _tokenIdCounter; //Keeps track of the current token to be made
        _tokenIdCounter += 1; //Increments token counter

        _mint(recipient, tokenId); //Mints a new NFT to the recipient with the token ID
        _setTokenURI(tokenId, tokenURI); //Sets the token URI to the newly created NFT token
    }
}