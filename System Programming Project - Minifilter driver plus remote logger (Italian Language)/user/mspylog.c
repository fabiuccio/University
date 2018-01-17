/*++
Questo è il codice della DLL che viene importata dal servizio per essere collegato al Minispy.
--*/

#pragma warning(disable : 4995)
#pragma warning(disable : 4142)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include <tchar.h>
#include <assert.h>
#include <strsafe.h>

#include "mspyLog.h"

#define TIME_BUFFER_LENGTH 20
#define TIME_TO_PRINT 12
#define POLL_INTERVAL   200
#define SUCCESS              0
#define USAGE_ERROR          1
#define EXIT_INTERPRETER     2
#define EXIT_PROGRAM         4
#define CMDLINE_SIZE              256
#define NUM_PARAMS                40

#define MINISPY_NAME            L"MiniSpy"

FormatSystemTime(
    __in SYSTEMTIME *SystemTime,
    __in_bcount(BufferLength) CHAR *Buffer,
    __in ULONG BufferLength
    );

VOID
PrintIrpCode(
    __in UCHAR MajorCode,
    __in UCHAR MinorCode,
    __inout UCHAR *OutputBuffer,
    __in BOOLEAN PrintMajorCode
);

VOID
MulticastDump(
    __in ULONG SequenceNumber,
    __in WCHAR CONST *Name,
    __in PRECORD_DATA RecordData,
	__inout UCHAR *packet
    );


//Entry main della DLL

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


//Funzione che viene chiamata per connettersi al MiniSpy.
//Ritorna l'HANDLE ad esso se lo trova.
HANDLE Connect()

{
	HANDLE p;
    HRESULT hResult = S_OK;
	hResult = FilterConnectCommunicationPort( MINISPY_PORT_NAME,
                                              0,
                                              NULL,
                                              0,
                                              NULL,
                                              &p );

    if (IS_ERROR( hResult )) {
        return (HANDLE)INVALID_HANDLE_VALUE;
    }
	else
		return p;
}

//Questa funzione viene utilizzata per agganciare un volume al Minispy. Riceve in ingresso una stringa, del tipo "c:"
//che rappresenta il volume che si vuole attaccare al filtro. Ritorna un Booleano a seconda che l'operazione sia
//Andata a buon fine oppure no.

BOOL
AttachDevice (
    __in LPCSTR AttachName
    )

{
	CHAR buffer[BUFFER_SIZE];
    DWORD bufferLength;
	HRESULT hResult;
    WCHAR instanceName[INSTANCE_NAME_MAX_CHARS + 1];

    bufferLength = MultiByteToWideChar( CP_ACP,
                                        MB_ERR_INVALID_CHARS,
                                        AttachName,
                                        -1,
                                        (LPWSTR)buffer,
                                        BUFFER_SIZE/sizeof( WCHAR ) );

    hResult = FilterAttach( MINISPY_NAME,
                            (PWSTR)buffer,
                            NULL, // instance name
                            sizeof( instanceName ),
                            instanceName );

    if (SUCCEEDED( hResult )) return TRUE;
        
    else return FALSE;

}

//Funzione duale della precedente: serve per disconnettere un device dal filtro.
BOOL
DetachDevice (
    __in LPCSTR DetachName
    )

{

    DWORD bufferLength;
	HRESULT hResult;
    PWCHAR instanceString;
    WCHAR instanceName[INSTANCE_NAME_MAX_CHARS + 1];

	bufferLength = MultiByteToWideChar( CP_ACP,
												MB_ERR_INVALID_CHARS,
												DetachName,
												-1,
												(LPWSTR)instanceName,
												sizeof( instanceName )/sizeof( WCHAR ) );

                        instanceString = instanceName;

    hResult = FilterDetach( MINISPY_NAME,
                                        (PWSTR)instanceName,
										 NULL);


    if (IS_ERROR( hResult )) return FALSE;
        
    else return TRUE;

}


BOOLEAN
TranslateFileTag(
    __in PLOG_RECORD logRecord
    )

{
    PFLT_TAG_DATA_BUFFER TagData;
    ULONG Length;
    TagData = (PFLT_TAG_DATA_BUFFER) &logRecord->Name[0];
    if (TagData->FileTag == IO_REPARSE_TAG_MOUNT_POINT) {

        Length = min( MAX_NAME_SPACE - sizeof(UNICODE_NULL), TagData->MountPointReparseBuffer.SubstituteNameLength );
        MoveMemory( &logRecord->Name[0],
                    TagData->MountPointReparseBuffer.PathBuffer,
                    Length );

        logRecord->Name[Length/sizeof(WCHAR)] = UNICODE_NULL;
        return TRUE;
    }
    return FALSE;
}

//Questa funzione serve ad ottenere i pacchetti. Il servizio, chiamando questa funzione, ottiene che la DLL
//funziona come ponte tra il minispy ed il servizio stesso.
VOID 
WINAPI 
GetPacket(
    __in LPVOID lpParameter,
	__inout UCHAR* packet
    )

{
    HANDLE port = (HANDLE)lpParameter;
    DWORD bytesReturned = 0;
    DWORD used;
    PVOID alignedBuffer[BUFFER_SIZE/sizeof( PVOID )];
    PCHAR buffer = (PCHAR) alignedBuffer;
    HRESULT hResult;
    PLOG_RECORD pLogRecord;
    PRECORD_DATA pRecordData;
    COMMAND_MESSAGE commandMessage;
	int i;
	int c=0;

    while (c<1) {


        commandMessage.Command = GetMiniSpyLog;

        hResult = FilterSendMessage( port,
                                     &commandMessage,
                                     sizeof( COMMAND_MESSAGE ),
                                     buffer,
                                     sizeof(alignedBuffer),
                                     &bytesReturned );

        if (IS_ERROR( hResult )) {
            if (HRESULT_FROM_WIN32( ERROR_INVALID_HANDLE ) == hResult) {
                printf( "The kernel component of minispy has unloaded. Exiting\n" );
                ExitProcess( 0 );
            } else {
                if (hResult != HRESULT_FROM_WIN32( ERROR_NO_MORE_ITEMS )) {
                    printf( "UNEXPECTED ERROR received: %x\n", hResult );
                }
                Sleep( POLL_INTERVAL );
            }

            continue;
        }


        pLogRecord = (PLOG_RECORD) buffer;
        used = 0;

        //for (;;) {
            if (used+FIELD_OFFSET(LOG_RECORD,Name) > bytesReturned) {
				
                break;
            }

            if (pLogRecord->Length < (sizeof(LOG_RECORD)+sizeof(WCHAR))) {
                break;
            }

            used += pLogRecord->Length;

            if (used > bytesReturned) {
                break;
            }

            pRecordData = &pLogRecord->Data;

            if (FlagOn(pLogRecord->RecordType,RECORD_TYPE_FILETAG)) {
                if (!TranslateFileTag( pLogRecord )){

                    pLogRecord = (PLOG_RECORD)Add2Ptr(pLogRecord,pLogRecord->Length);
                    continue;
                }
            }
			
			MulticastDump(  pLogRecord->SequenceNumber,
							pLogRecord->Name,
							pRecordData,
							packet);

            pLogRecord = (PLOG_RECORD)Add2Ptr(pLogRecord,pLogRecord->Length);
        //}
		c++;
    }
}

//Questa funzione serve a stampare i nomi relativi ai codici IRP
VOID
PrintIrpCode(
    __in UCHAR MajorCode,
    __in UCHAR MinorCode,
    __inout UCHAR *OutputBuffer,
    __in BOOLEAN PrintMajorCode
)

{
    UCHAR *irpMajorString, *irpMinorString = NULL;
    UCHAR errorBuf[128];
	UCHAR zero = 0;

    switch (MajorCode) {
        case IRP_MJ_CREATE:
            irpMajorString = IRP_MJ_CREATE_STRING;
            break;
        case IRP_MJ_CREATE_NAMED_PIPE:
            irpMajorString = IRP_MJ_CREATE_NAMED_PIPE_STRING;
            break;
        case IRP_MJ_CLOSE:
            irpMajorString = IRP_MJ_CLOSE_STRING;
            break;
        case IRP_MJ_READ:
            irpMajorString = IRP_MJ_READ_STRING;
            switch (MinorCode) {
                case IRP_MN_NORMAL:
                    irpMinorString = IRP_MN_NORMAL_STRING;
                    break;
                case IRP_MN_DPC:
                    irpMinorString = IRP_MN_DPC_STRING;
                    break;
                case IRP_MN_MDL:
                    irpMinorString = IRP_MN_MDL_STRING;
                    break;
                case IRP_MN_COMPLETE:
                    irpMinorString = IRP_MN_COMPLETE_STRING;
                    break;
                case IRP_MN_COMPRESSED:
                    irpMinorString = IRP_MN_COMPRESSED_STRING;
                    break;
                case IRP_MN_MDL_DPC:
                    irpMinorString = IRP_MN_MDL_DPC_STRING;
                    break;
                case IRP_MN_COMPLETE_MDL:
                    irpMinorString = IRP_MN_COMPLETE_MDL_STRING;
                    break;
                case IRP_MN_COMPLETE_MDL_DPC:
                    irpMinorString = IRP_MN_COMPLETE_MDL_DPC_STRING;
                    break;
                default:
                    sprintf_s(errorBuf, sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_WRITE:
            irpMajorString = IRP_MJ_WRITE_STRING;
            switch (MinorCode) {
                case IRP_MN_NORMAL:
                    irpMinorString = IRP_MN_NORMAL_STRING;
                    break;
                case IRP_MN_DPC:
                    irpMinorString = IRP_MN_DPC_STRING;
                    break;
                case IRP_MN_MDL:
                    irpMinorString = IRP_MN_MDL_STRING;
                    break;
                case IRP_MN_COMPLETE:
                    irpMinorString = IRP_MN_COMPLETE_STRING;
                    break;
                case IRP_MN_COMPRESSED:
                    irpMinorString = IRP_MN_COMPRESSED_STRING;
                    break;
                case IRP_MN_MDL_DPC:
                    irpMinorString = IRP_MN_MDL_DPC_STRING;
                    break;
                case IRP_MN_COMPLETE_MDL:
                    irpMinorString = IRP_MN_COMPLETE_MDL_STRING;
                    break;
                case IRP_MN_COMPLETE_MDL_DPC:
                    irpMinorString = IRP_MN_COMPLETE_MDL_DPC_STRING;
                    break;
                default:
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_QUERY_INFORMATION:
            irpMajorString = IRP_MJ_QUERY_INFORMATION_STRING;
            break;
        case IRP_MJ_SET_INFORMATION:
            irpMajorString = IRP_MJ_SET_INFORMATION_STRING;
            break;
        case IRP_MJ_QUERY_EA:
            irpMajorString = IRP_MJ_QUERY_EA_STRING;
            break;
        case IRP_MJ_SET_EA:
            irpMajorString = IRP_MJ_SET_EA_STRING;
            break;
        case IRP_MJ_FLUSH_BUFFERS:
            irpMajorString = IRP_MJ_FLUSH_BUFFERS_STRING;
            break;
        case IRP_MJ_QUERY_VOLUME_INFORMATION:
            irpMajorString = IRP_MJ_QUERY_VOLUME_INFORMATION_STRING;
            break;
        case IRP_MJ_SET_VOLUME_INFORMATION:
            irpMajorString = IRP_MJ_SET_VOLUME_INFORMATION_STRING;
            break;
        case IRP_MJ_DIRECTORY_CONTROL:
            irpMajorString = IRP_MJ_DIRECTORY_CONTROL_STRING;
            switch (MinorCode) {
                case IRP_MN_QUERY_DIRECTORY:
                    irpMinorString = IRP_MN_QUERY_DIRECTORY_STRING;
                    break;
                case IRP_MN_NOTIFY_CHANGE_DIRECTORY:
                    irpMinorString = IRP_MN_NOTIFY_CHANGE_DIRECTORY_STRING;
                    break;
                default:
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_FILE_SYSTEM_CONTROL:
            irpMajorString = IRP_MJ_FILE_SYSTEM_CONTROL_STRING;
            switch (MinorCode) {
                case IRP_MN_USER_FS_REQUEST:
                    irpMinorString = IRP_MN_USER_FS_REQUEST_STRING;
                    break;
                case IRP_MN_MOUNT_VOLUME:
                    irpMinorString = IRP_MN_MOUNT_VOLUME_STRING;
                    break;
                case IRP_MN_VERIFY_VOLUME:
                    irpMinorString = IRP_MN_VERIFY_VOLUME_STRING;
                    break;
                case IRP_MN_LOAD_FILE_SYSTEM:
                    irpMinorString = IRP_MN_LOAD_FILE_SYSTEM_STRING;
                    break;
                case IRP_MN_TRACK_LINK:
                    irpMinorString = IRP_MN_TRACK_LINK_STRING;
                    break;
                default:
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_DEVICE_CONTROL:
            irpMajorString = IRP_MJ_DEVICE_CONTROL_STRING;
            switch (MinorCode) {
                case IRP_MN_SCSI_CLASS:
                    irpMinorString = IRP_MN_SCSI_CLASS_STRING;
                    break;
                default:
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_INTERNAL_DEVICE_CONTROL:
            irpMajorString = IRP_MJ_INTERNAL_DEVICE_CONTROL_STRING;
            break;
        case IRP_MJ_SHUTDOWN:
            irpMajorString = IRP_MJ_SHUTDOWN_STRING;
            break;
        case IRP_MJ_LOCK_CONTROL:
            irpMajorString = IRP_MJ_LOCK_CONTROL_STRING;
            switch (MinorCode) {
                case IRP_MN_LOCK:
                    irpMinorString = IRP_MN_LOCK_STRING;
                    break;
                case IRP_MN_UNLOCK_SINGLE:
                    irpMinorString = IRP_MN_UNLOCK_SINGLE_STRING;
                    break;
                case IRP_MN_UNLOCK_ALL:
                    irpMinorString = IRP_MN_UNLOCK_ALL_STRING;
                    break;
                case IRP_MN_UNLOCK_ALL_BY_KEY:
                    irpMinorString = IRP_MN_UNLOCK_ALL_BY_KEY_STRING;
                    break;
                default:
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_CLEANUP:
            irpMajorString = IRP_MJ_CLEANUP_STRING;
            break;
        case IRP_MJ_CREATE_MAILSLOT:
            irpMajorString = IRP_MJ_CREATE_MAILSLOT_STRING;
            break;
        case IRP_MJ_QUERY_SECURITY:
            irpMajorString = IRP_MJ_QUERY_SECURITY_STRING;
            break;
        case IRP_MJ_SET_SECURITY:
            irpMajorString = IRP_MJ_SET_SECURITY_STRING;
            break;
        case IRP_MJ_POWER:
            irpMajorString = IRP_MJ_POWER_STRING;
            switch (MinorCode) {
                case IRP_MN_WAIT_WAKE:
                    irpMinorString = IRP_MN_WAIT_WAKE_STRING;
                    break;
                case IRP_MN_POWER_SEQUENCE:
                    irpMinorString = IRP_MN_POWER_SEQUENCE_STRING;
                    break;
                case IRP_MN_SET_POWER:
                    irpMinorString = IRP_MN_SET_POWER_STRING;
                    break;
                case IRP_MN_QUERY_POWER:
                    irpMinorString = IRP_MN_QUERY_POWER_STRING;
                    break;
                default :
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_SYSTEM_CONTROL:
            irpMajorString = IRP_MJ_SYSTEM_CONTROL_STRING;
            switch (MinorCode) {
                case IRP_MN_QUERY_ALL_DATA:
                    irpMinorString = IRP_MN_QUERY_ALL_DATA_STRING;
                    break;
                case IRP_MN_QUERY_SINGLE_INSTANCE:
                    irpMinorString = IRP_MN_QUERY_SINGLE_INSTANCE_STRING;
                    break;
                case IRP_MN_CHANGE_SINGLE_INSTANCE:
                    irpMinorString = IRP_MN_CHANGE_SINGLE_INSTANCE_STRING;
                    break;
                case IRP_MN_CHANGE_SINGLE_ITEM:
                    irpMinorString = IRP_MN_CHANGE_SINGLE_ITEM_STRING;
                    break;
                case IRP_MN_ENABLE_EVENTS:
                    irpMinorString = IRP_MN_ENABLE_EVENTS_STRING;
                    break;
                case IRP_MN_DISABLE_EVENTS:
                    irpMinorString = IRP_MN_DISABLE_EVENTS_STRING;
                    break;
                case IRP_MN_ENABLE_COLLECTION:
                    irpMinorString = IRP_MN_ENABLE_COLLECTION_STRING;
                    break;
                case IRP_MN_DISABLE_COLLECTION:
                    irpMinorString = IRP_MN_DISABLE_COLLECTION_STRING;
                    break;
                case IRP_MN_REGINFO:
                    irpMinorString = IRP_MN_REGINFO_STRING;
                    break;
                case IRP_MN_EXECUTE_METHOD:
                    irpMinorString = IRP_MN_EXECUTE_METHOD_STRING;
                    break;
                default :
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        case IRP_MJ_DEVICE_CHANGE:
            irpMajorString = IRP_MJ_DEVICE_CHANGE_STRING;
            break;
        case IRP_MJ_QUERY_QUOTA:
            irpMajorString = IRP_MJ_QUERY_QUOTA_STRING;
            break;
        case IRP_MJ_SET_QUOTA:
            irpMajorString = IRP_MJ_SET_QUOTA_STRING;
            break;
        case IRP_MJ_PNP:
            irpMajorString = IRP_MJ_PNP_STRING;
            switch (MinorCode) {
                case IRP_MN_START_DEVICE:
                    irpMinorString = IRP_MN_START_DEVICE_STRING;
                    break;
                case IRP_MN_QUERY_REMOVE_DEVICE:
                    irpMinorString = IRP_MN_QUERY_REMOVE_DEVICE_STRING;
                    break;
                case IRP_MN_REMOVE_DEVICE:
                    irpMinorString = IRP_MN_REMOVE_DEVICE_STRING;
                    break;
                case IRP_MN_CANCEL_REMOVE_DEVICE:
                    irpMinorString = IRP_MN_CANCEL_REMOVE_DEVICE_STRING;
                    break;
                case IRP_MN_STOP_DEVICE:
                    irpMinorString = IRP_MN_STOP_DEVICE_STRING;
                    break;
                case IRP_MN_QUERY_STOP_DEVICE:
                    irpMinorString = IRP_MN_QUERY_STOP_DEVICE_STRING;
                    break;
                case IRP_MN_CANCEL_STOP_DEVICE:
                    irpMinorString = IRP_MN_CANCEL_STOP_DEVICE_STRING;
                    break;
                case IRP_MN_QUERY_DEVICE_RELATIONS:
                    irpMinorString = IRP_MN_QUERY_DEVICE_RELATIONS_STRING;
                    break;
                case IRP_MN_QUERY_INTERFACE:
                    irpMinorString = IRP_MN_QUERY_INTERFACE_STRING;
                    break;
                case IRP_MN_QUERY_CAPABILITIES:
                    irpMinorString = IRP_MN_QUERY_CAPABILITIES_STRING;
                    break;
                case IRP_MN_QUERY_RESOURCES:
                    irpMinorString = IRP_MN_QUERY_RESOURCES_STRING;
                    break;
                case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
                    irpMinorString = IRP_MN_QUERY_RESOURCE_REQUIREMENTS_STRING;
                    break;
                case IRP_MN_QUERY_DEVICE_TEXT:
                    irpMinorString = IRP_MN_QUERY_DEVICE_TEXT_STRING;
                    break;
                case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
                    irpMinorString = IRP_MN_FILTER_RESOURCE_REQUIREMENTS_STRING;
                    break;
                case IRP_MN_READ_CONFIG:
                    irpMinorString = IRP_MN_READ_CONFIG_STRING;
                    break;
                case IRP_MN_WRITE_CONFIG:
                    irpMinorString = IRP_MN_WRITE_CONFIG_STRING;
                    break;
                case IRP_MN_EJECT:
                    irpMinorString = IRP_MN_EJECT_STRING;
                    break;
                case IRP_MN_SET_LOCK:
                    irpMinorString = IRP_MN_SET_LOCK_STRING;
                    break;
                case IRP_MN_QUERY_ID:
                    irpMinorString = IRP_MN_QUERY_ID_STRING;
                    break;
                case IRP_MN_QUERY_PNP_DEVICE_STATE:
                    irpMinorString = IRP_MN_QUERY_PNP_DEVICE_STATE_STRING;
                    break;
                case IRP_MN_QUERY_BUS_INFORMATION:
                    irpMinorString = IRP_MN_QUERY_BUS_INFORMATION_STRING;
                    break;
                case IRP_MN_DEVICE_USAGE_NOTIFICATION:
                    irpMinorString = IRP_MN_DEVICE_USAGE_NOTIFICATION_STRING;
                    break;
                case IRP_MN_SURPRISE_REMOVAL:
                    irpMinorString = IRP_MN_SURPRISE_REMOVAL_STRING;
                    break;
                case IRP_MN_QUERY_LEGACY_BUS_INFORMATION:
                    irpMinorString = IRP_MN_QUERY_LEGACY_BUS_INFORMATION_STRING;
                    break;
                default :
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp minor code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;


        case IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION:
            irpMajorString = IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION_STRING;
            break;

        case IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION:
            irpMajorString = IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION_STRING;
            break;

        case IRP_MJ_ACQUIRE_FOR_MOD_WRITE:
            irpMajorString = IRP_MJ_ACQUIRE_FOR_MOD_WRITE_STRING;
            break;

        case IRP_MJ_RELEASE_FOR_MOD_WRITE:
            irpMajorString = IRP_MJ_RELEASE_FOR_MOD_WRITE_STRING;
            break;

        case IRP_MJ_ACQUIRE_FOR_CC_FLUSH:
            irpMajorString = IRP_MJ_ACQUIRE_FOR_CC_FLUSH_STRING;
            break;

        case IRP_MJ_RELEASE_FOR_CC_FLUSH:
            irpMajorString = IRP_MJ_RELEASE_FOR_CC_FLUSH_STRING;
            break;

        case IRP_MJ_NOTIFY_STREAM_FO_CREATION:
            irpMajorString = IRP_MJ_NOTIFY_STREAM_FO_CREATION_STRING;
            break;

        case IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE:
            irpMajorString = IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE_STRING;
            break;

        case IRP_MJ_NETWORK_QUERY_OPEN:
            irpMajorString = IRP_MJ_NETWORK_QUERY_OPEN_STRING;
            break;

        case IRP_MJ_MDL_READ:
            irpMajorString = IRP_MJ_MDL_READ_STRING;
            break;

        case IRP_MJ_MDL_READ_COMPLETE:
            irpMajorString = IRP_MJ_MDL_READ_COMPLETE_STRING;
            break;

        case IRP_MJ_PREPARE_MDL_WRITE:
            irpMajorString = IRP_MJ_PREPARE_MDL_WRITE_STRING;
            break;

        case IRP_MJ_MDL_WRITE_COMPLETE:
            irpMajorString = IRP_MJ_MDL_WRITE_COMPLETE_STRING;
            break;

        case IRP_MJ_VOLUME_MOUNT:
            irpMajorString = IRP_MJ_VOLUME_MOUNT_STRING;
            break;

        case IRP_MJ_VOLUME_DISMOUNT:
            irpMajorString = IRP_MJ_VOLUME_DISMOUNT_STRING;
            break;

        case IRP_MJ_TRANSACTION_NOTIFY:
            irpMajorString = IRP_MJ_TRANSACTION_NOTIFY_STRING;
            switch (MinorCode) {
                case 0:
                    irpMinorString = TRANSACTION_BEGIN;
                    break;
                case TRANSACTION_NOTIFY_PREPREPARE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_PREPREPARE_STRING;
                    break;
                case TRANSACTION_NOTIFY_PREPARE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_PREPARE_STRING;
                    break;
                case TRANSACTION_NOTIFY_COMMIT_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_COMMIT_STRING;
                    break;
                case TRANSACTION_NOTIFY_COMMIT_FINALIZE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_COMMIT_FINALIZE_STRING;
                    break;
                case TRANSACTION_NOTIFY_ROLLBACK_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_ROLLBACK_STRING;
                    break;
                case TRANSACTION_NOTIFY_PREPREPARE_COMPLETE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_PREPREPARE_COMPLETE_STRING;
                    break;
                case TRANSACTION_NOTIFY_PREPARE_COMPLETE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_COMMIT_COMPLETE_STRING;
                    break;
                case TRANSACTION_NOTIFY_ROLLBACK_COMPLETE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_ROLLBACK_COMPLETE_STRING;
                    break;
                case TRANSACTION_NOTIFY_RECOVER_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_RECOVER_STRING;
                    break;
                case TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT_STRING;
                    break;
                case TRANSACTION_NOTIFY_DELEGATE_COMMIT_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_DELEGATE_COMMIT_STRING;
                    break;
                case TRANSACTION_NOTIFY_RECOVER_QUERY_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_RECOVER_QUERY_STRING;
                    break;
                case TRANSACTION_NOTIFY_ENLIST_PREPREPARE_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_ENLIST_PREPREPARE_STRING;
                    break;
                case TRANSACTION_NOTIFY_LAST_RECOVER_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_LAST_RECOVER_STRING;
                    break;
                case TRANSACTION_NOTIFY_INDOUBT_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_INDOUBT_STRING;
                    break;
                case TRANSACTION_NOTIFY_PROPAGATE_PULL_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_PROPAGATE_PULL_STRING;
                    break;
                case TRANSACTION_NOTIFY_PROPAGATE_PUSH_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_PROPAGATE_PUSH_STRING;
                    break;
                case TRANSACTION_NOTIFY_MARSHAL_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_MARSHAL_STRING;
                    break;
                case TRANSACTION_NOTIFY_ENLIST_MASK_CODE:
                    irpMinorString = TRANSACTION_NOTIFY_ENLIST_MASK_STRING;
                    break;
                default:
                    sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Transaction notication code (%u)",MinorCode);
                    irpMinorString = errorBuf;
            }
            break;

        default:
            sprintf_s(errorBuf,sizeof(errorBuf),"Unknown Irp major function (%d)",MajorCode);
            irpMajorString = errorBuf;
            break;
    }

	if (irpMinorString) {
		sprintf(OutputBuffer, "%s%c%s", irpMajorString, zero, irpMinorString);
    } else {
		sprintf(OutputBuffer, "%s", irpMajorString);
    }
}

//Questa funzione serve ad eseguire la formattazione del tempo che viene inviato nei acchetti al servizio
ULONG
FormatSystemTime(
    __in SYSTEMTIME *SystemTime,
    __in_bcount(BufferLength) CHAR *Buffer,
    __in ULONG BufferLength
    )
{
    CHAR *writePosition;
    ULONG returnLength = 0;

    writePosition = Buffer;

    if (BufferLength < TIME_BUFFER_LENGTH) {
        return 0;
    }

    returnLength = sprintf_s( Buffer,
                            BufferLength,
                            "%02d:%02d:%02d:%03d",
                            SystemTime->wHour,
                            SystemTime->wMinute,
                            SystemTime->wSecond,
                            SystemTime->wMilliseconds );

    return returnLength;
}


VOID
MulticastDump(
    __in ULONG SequenceNumber,
    __in WCHAR CONST *Name,
    __in PRECORD_DATA RecordData,
	__inout UCHAR *buffer
    )
{
    FILETIME localTime;
    SYSTEMTIME systemTime;
    CHAR time[TIME_BUFFER_LENGTH];
	UINT i=0;
	DWORD dimName=0;
	ULONG timeLength = 0;
	HMODULE hMod;
	DWORD cbNeeded;
	HANDLE hProcessSnap;

	memcpy(buffer, &SequenceNumber, sizeof(SequenceNumber));

    FileTimeToLocalFileTime( (FILETIME *)&(RecordData->OriginatingTime),
                             &localTime );
    FileTimeToSystemTime( &localTime,
                          &systemTime );

    if (timeLength=FormatSystemTime( &systemTime, time, TIME_BUFFER_LENGTH )) {
        memcpy(buffer+sizeof(SequenceNumber), time, timeLength);
    } else {
		memset(buffer+sizeof(SequenceNumber), 0, timeLength);
    }


	dimName=WideCharToMultiByte(CP_UTF8, 0, Name, -1, buffer+sizeof(SequenceNumber)
											   + timeLength,
											   1024-sizeof(SequenceNumber)
											   -timeLength
											, NULL, NULL); 

	memcpy(buffer+sizeof(SequenceNumber)+timeLength+dimName,&(RecordData->ProcessId),4);

    PrintIrpCode( RecordData->CallbackMajorId,
                  RecordData->CallbackMinorId,
                  buffer+sizeof(SequenceNumber)+timeLength+4+dimName,
                  TRUE);
}
