Action:GetRequest                     Source:MaidNode:Single          Destination:DataManager:Group     Contents:nfs::DataName                    HasResponse
Action:PutRequest                     Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::DataAndPmidHint             HasResponse
Action:DeleteRequest                  Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::DataName
Action:GetVersionsRequest             Source:MaidNode:Single          Destination:VersionManager:Group  Contents:nfs::DataName                    HasResponse
Action:GetBranchRequest               Source:MaidNode:Single          Destination:VersionManager:Group  Contents:nfs::DataNameAndVersion          HasResponse
Action:PutVersionRequest              Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::DataNameOldNewVersion       HasResponse
Action:DeleteBranchUntilForkRequest   Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::DataNameAndVersion
Action:CreateAccountRequest           Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::Empty                       HasResponse
Action:RemoveAccountRequest           Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::Empty                       HasResponse
Action:RegisterPmidRequest            Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::PmidRegistration            HasResponse
Action:UnregisterPmidRequest          Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::PmidRegistration            HasResponse
Action:GetPmidHealthRequest           Source:MaidNode:Single          Destination:MaidManager:Group     Contents:nfs::DataName                    HasResponse

Action:GetResponse                    Source:MaidManager:Group        Destination:MaidNode:Single       Contents:nfs::DataAndReturnCode
Action:PutResponse                    Source:MaidManager:Group        Destination:MaidNode:Single       Contents:nfs::DataPmidHintAndReturnCode
Action:PutRequest                     Source:MaidManager:Group        Destination:DataManager:Group     Contents:nfs::DataAndPmidHint             HasResponse
Action:DeleteRequest                  Source:MaidManager:Group        Destination:DataManager:Group     Contents:nfs::DataName                    HasResponse

Action:PutRequest                     Source:PmidManager:Group        Destination:PmidNode:Single       Contents:nfs::DataNameAndContent           HasResponse
Action:DeleteRequest                  Source:PmidManager:Group        Destination:PmidNode:Single       Contents:nfs::DataName

Action:PutRequest                     Source:DataManager:Group        Destination:PmidManager:Group     Contents:nfs::DataNameAndContent           HasResponse
Action:GetRequest                     Source:DataManager:Group        Destination:PmidNode:Single       Contents:nfs::DataName                     HasResponse
Action:GetResponse                    Source:DataManager:Group        Destination:MaidNode:Single       Contents:nfs::DataOrDataNameAndReturnCode
